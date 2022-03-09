#include "noisegate.h"

void NoiseGate::setThresh(uint16_t thresh) {
    if (thresh < 0 || thresh > INT16_MAX) {
      return;
    }
    _thresh = thresh;
}

void NoiseGate::update(void) {
    audio_block_t *block;
    block = receiveWritable();
    if (!block) {
        return; // did not receive an allocated block
    }

    
    for (int16_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        int16_t x = block->data[i];
        // anything below threshold becomes 0
        block->data[i] = (abs(x) > _thresh) ? x : 0;
    }

    transmit(block);
    release(block);
}
