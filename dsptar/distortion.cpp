#include "distortion.h"

Distortion::~Distortion() {
    if (_distortionArr) {
        delete[] _distortionArr;
    }
}

bool Distortion::setup(const int16_t *distortionArr, int length) {
    // famous trick, if (n) & (n - 1) == 0 then n is a power of 2
    bool isPowerOfTwoPlusOne = ((length - 1) & (length - 2)) == 0;
    if (!isPowerOfTwoPlusOne) {
        return false;
    }

    if (length <= (1 << 0) + 1 || length >= (1 << 15) + 1) {
        return false;
    }

    _distortionArr = distortionArr;

    // find the number of bits to shift when interpolating
    int lengthMinusOne = length - 1;
    _bitShift = 16;
    while (lengthMinusOne >>= 1) {
        _bitShift--;
    }

    return true;
}

void Distortion::update(void) {
    // not setup
    if (!_distortionArr) {
        return;
    }

    audio_block_t *block;
    block = receiveWritable();
    if (!block) {
        return; // did not receive an allocated block
    }

    
    for (int16_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        // data ranges from -32768 to 32767. convert to 0 to 65535
        uint16_t x = block->data[i] - INT16_MIN; // aka + 32768
        // linear interpolation
        uint16_t arr_idx = x >> _bitShift; // find associated index in distortion array
        int16_t y_lower = _distortionArr[arr_idx]; 
        int16_t y_upper = _distortionArr[arr_idx + 1];
        // need 32 bits to store result of 16 bit multiplication
        int32_t raw = y_lower + ((y_upper - y_lower) * (x - (arr_idx << _bitShift)) >> _bitShift);

        // clip at int16 min/max
        block->data[i] = (int16_t) constrain(raw, INT16_MIN, INT16_MAX);
    }

    transmit(block);
    release(block);
}
