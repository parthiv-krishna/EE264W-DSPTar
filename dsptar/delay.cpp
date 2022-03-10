#include "delay.h"

void Delay::setDelay(int index, int delayMs, int16_t log2Attenuation) {
    if (index < 0 || index >= NUM_DELAY_TAPS || delayMs < 0) {
        return;
    }
    uint32_t delaySamples = delayMs * AUDIO_SAMPLE_RATE / 1000;
    uint32_t delayBlocks = delaySamples / AUDIO_BLOCK_SAMPLES;
    if (delayBlocks > DELAY_QUEUE_SIZE) {
        delayBlocks = 0;
    }
    _delays[index].delayBlocks = delayBlocks;
    _delays[index].log2Attenuation = log2Attenuation;
}

void Delay::update() {
    audio_block_t *block;
    block = receiveWritable();
    if (!block) {
        return; // did not receive an allocated block
    }

    audio_block_t *out;
    out = allocate();
    if (!out) {
        return; //  did not allocate an output block
    }

    memcpy(out->data, block->data, AUDIO_BLOCK_SAMPLES * sizeof(int16_t));

    for (int delayTap = 0; delayTap < NUM_DELAY_TAPS; delayTap++) {
        if (_delays[delayTap].delayBlocks > 0) {
            audio_block_t *delayBlock = _delayQueue.peekFront(_delays[delayTap].delayBlocks);
            if (delayBlock) {
                for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
                    int32_t acc = out->data[i] + (delayBlock->data[i] >> _delays[delayTap].log2Attenuation);
                    out->data[i] = (int16_t) constrain(acc, INT16_MIN, INT16_MAX);
                }
            }
        } 
    }

    audio_block_t* toFree = _delayQueue.push(block); // will give block that we need to free
    if (toFree) {
        release(toFree);
    }

    transmit(out);
    release(out);
}
