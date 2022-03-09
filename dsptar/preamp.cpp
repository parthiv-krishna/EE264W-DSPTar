#include "preamp.h"

void Preamp::setGain(float gain) {
    _gain = gain;
}

void Preamp::update(void) {
    audio_block_t *block;
    block = receiveWritable();
    if (!block) {
        return; // did not receive an allocated block
    }

    
    for (int16_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        int16_t x = block->data[i];
        float raw = x * _gain;
        // clip at int16 min/max
        block->data[i] = (int16_t) constrain(raw, INT16_MIN, INT16_MAX);
    }

    transmit(block);
    release(block);
}
