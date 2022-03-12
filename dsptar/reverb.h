#ifndef REVERB_H
#define REVERB_H

#include "Arduino.h"
#include "AudioStream.h"

#include "arm_math.h"
#include "arm_const_structs.h"

#define REVERB_MAX_NBLOCKS 128

/**
 * @brief An AudioStream object that applies a reverb effect
 *
 */
class Reverb : public AudioStream {
public:
    /**
     * @brief Construct a new Reverb object
     * 
     * Initializes the AudioStream super class to take in
     * two input blocks in inputQueueArray.
     * 
     */
    Reverb(void) : AudioStream(2, inputQueueArray) {}

    /**
     * @brief Sets up the Reverb object with a given array
     * 
     * @param gain Gain to apply to the reverb
     * @param fftCoeffs Statically declared array float32 array of size nBlocks * REVERB_FFT_LENGTH * 2, updated to store fft of irArr
     * @param nBlocks Length of irArr in terms of audio blocks
     * @param irArr Impulse response to convolve with signal
     * @param irArrLen Length of impulse response array in samples
     * @param fftTemp 
     * @return true 
     * @return false 
     */
    bool setup(float32_t gain, float32_t *fftCoeffs, const float32_t *irArr, float32_t irArrLen, float32_t *fftTemp);
    
    /**
     * @brief Transmits two output blocks after applying reverb 
     * 
     */
    virtual void update(void);

private:
    audio_block_t *inputQueueArray[2];

    bool _setup = false;
    size_t _buffIdx;
    size_t _currIdx;

    float32_t _gain; // additional gain factor

    float32_t _freqResponse[REVERB_MAX_NBLOCKS][AUDIO_BLOCK_SAMPLES * 4];
    float32_t _fftin[AUDIO_BLOCK_SAMPLES * 4];
    float32_t _floatBufL[AUDIO_BLOCK_SAMPLES];
    float32_t _floatBufR[AUDIO_BLOCK_SAMPLES];
    float32_t _lastBlockL[AUDIO_BLOCK_SAMPLES];
    float32_t _lastBlockR[AUDIO_BLOCK_SAMPLES];
    float32_t _acc[AUDIO_BLOCK_SAMPLES * 4];
    float32_t _acc2[AUDIO_BLOCK_SAMPLES * 4];
    float32_t *_ptr_freqResponse;
    float32_t *_ptr_fftout;
    float32_t *_ptr_fftout_curr;
    float32_t *_ptr_freqResponse_curr;
    
    int _nBlocks;
    
    // counters
    int _k;
    int _kMult;
    int _jMult;
};

#endif
