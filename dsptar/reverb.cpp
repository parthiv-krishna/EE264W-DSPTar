#include "reverb.h"

Reverb::~Reverb() {
    delete[] _irArr;
    _irArrLen = 0;
}

bool Reverb::setup(int16_t *impulseResponse, size_t length) {
    if (length % 2 || length < 4) {
        return false; // must be even-length of at least 4
    }
  
    if (_irArr) {
        delete[] _irArr;
    }
    if (_state) {
        delete[] _state;
    }

    _irArr = new int16_t[length];
    if (!_irArr) {
        return false;
    }
    _irArrLen = length;

    // copy array into our memory
    for (size_t i = 0; i < length; i++) {
        _irArr[i] = _irArr[i];
    }

    // initialize state to 0s
    _state = new q15_t[length + AUDIO_BLOCK_SAMPLES];
    if (!_state) {
        return false;
    }
    memset(_state, 0, sizeof(q15_t) * (length + AUDIO_BLOCK_SAMPLES));


    // initialize accelerator struct
    arm_status result = arm_fir_init_q15(&_fir, _irArrLen, _irArr,
                                         &_state[0], AUDIO_BLOCK_SAMPLES);

    return (result == ARM_MATH_SUCCESS);
}

void Reverb::update() {
    if (!_irArr || !_state) {
        return; // not setup
    }

    audio_block_t *block;
    block = receiveReadOnly();

    if (!block) {
        Serial.println("no block");
        return; // did not receive a block

    }

    audio_block_t *result;
    result = allocate();
    if (result) {
        arm_fir_q15(&_fir, (q15_t *)block->data,
                    (q15_t *)result->data, AUDIO_BLOCK_SAMPLES);
        transmit(result);
        release(result);
    }

    release(block);
}
