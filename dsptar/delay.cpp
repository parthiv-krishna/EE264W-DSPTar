#include "delay.h"

void Delay::setup(float maxSecs, size_t numTaps) {
    if (maxSecs < 0) {
        return;
    }

    // clean up old ones
    if (_delays) {
        delete[] _delays;
    }
    if (_delayQueue) {
        delete _delayQueue;
    }
    
    _delays = new delay_tap_t[numTaps];
    _numTaps = numTaps;

    int queueSize = (maxSecs * AUDIO_SAMPLE_RATE) / AUDIO_BLOCK_SAMPLES;
    _delayQueue = new RingBuffer<audio_block_t*>(queueSize, nullptr);
    
    for (size_t i = 0; i < numTaps; i++) {
        _delays[i].delayBlocks = 0;
        _delays[i].log2Attenuation = 0;
    }
}

void Delay::setDelay(size_t index, int delayMs, int16_t log2Attenuation) {
    if (index < 0 || index >= _numTaps || delayMs < 0) {
        return;
    }
    int delaySamples = delayMs * AUDIO_SAMPLE_RATE / 1000;
    int delayBlocks = delaySamples / AUDIO_BLOCK_SAMPLES;
    if (delayBlocks > _delayQueue->size()) {
        delayBlocks = 0;
    }
    _delays[index].delayBlocks = delayBlocks;
    _delays[index].log2Attenuation = log2Attenuation;
}

void Delay::update() {
    if (!_delays || !_delayQueue) {
        return; // not setup
    }
  
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

    for (size_t delayTap = 0; delayTap < _numTaps; delayTap++) {
        if (_delays[delayTap].delayBlocks > 0) {
            audio_block_t *delayBlock = _delayQueue->peekFront(_delays[delayTap].delayBlocks);
            if (delayBlock) {
                // only do this if we've collected enough blocks to start adding delay
                for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
                    int32_t acc = out->data[i] + (delayBlock->data[i] >> _delays[delayTap].log2Attenuation);
                    out->data[i] = (int16_t) constrain(acc, INT16_MIN, INT16_MAX);
                }
            }
        } 
    }

    audio_block_t* toFree;
    // if true then we need to free the evicted block
    if ( _delayQueue->push(block, &toFree)) {
        release(toFree);
    }

    transmit(out);
    release(out);
}
