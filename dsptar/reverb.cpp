#include "reverb.h"

bool Reverb::setup(float32_t gain, float32_t *fftCoeffs, const float32_t *irArr, float32_t irArrLen, float32_t *fftTemp) {
    // make sure that we don't try to compute reverb in the middle of setting up
    _setup = false;

    // reset state
    _buffIdx = 0;
    _k = 0;

    _gain = gain;
    int nBlocks = irArrLen / AUDIO_BLOCK_SAMPLES;
    if (nBlocks > REVERB_MAX_NBLOCKS) {
        nBlocks = REVERB_MAX_NBLOCKS;
    }
    _nBlocks = nBlocks; 

    // define the pointer that is used to access the main program DMAMEM array fftout[]
    _ptr_fftout = fftCoeffs;
    _ptr_freqResponse = &_freqResponse[0][0];

    memset(_ptr_fftout, 0, _nBlocks*AUDIO_BLOCK_SAMPLES*4*4);  // clear fftout array
    memset(_fftin, 0,  AUDIO_BLOCK_SAMPLES * 4 * 4);  // clear _fftin array

    const static arm_cfft_instance_f32* maskS;
    maskS = &arm_cfft_sR_f32_len256;


    // compute STFT of impulse response and store in fftCoeffs
    for (size_t j = 0; j < _nBlocks; j++) {
        memset(fftTemp, 0, AUDIO_BLOCK_SAMPLES * 4 * 4); // clear fftTemp
        for (size_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            size_t idx = i + j * AUDIO_BLOCK_SAMPLES;
            if (idx < irArrLen) {
                fftTemp[i * 2 + AUDIO_BLOCK_SAMPLES * 2] = irArr[idx];
            }
            else {
                // zero pad to whole number of blocks
                fftTemp[i * 2 + AUDIO_BLOCK_SAMPLES * 2] = 0.0;
            } 
        }
        // perform complex FFT on fftTemp
        arm_cfft_f32(maskS, fftTemp, 0, 1);
        // fill into _freqResponse array
        for (size_t i = 0; i < AUDIO_BLOCK_SAMPLES * 4; i++) {
            _freqResponse[j][i] = fftTemp[i];
        }
    }
    _setup = true; // enable 
    return true;
}


void Reverb::update(void) {
    audio_block_t *blockL, *blockR;
    int16_t *bp;
    if (!_setup) return;
    blockL = receiveWritable(0);
    blockR = receiveWritable(1);
    if (blockL && blockR) {
        bp = blockL->data;
        arm_q15_to_float(bp, _floatBufL, AUDIO_BLOCK_SAMPLES);
        bp = blockR->data;
        arm_q15_to_float(bp, _floatBufR, AUDIO_BLOCK_SAMPLES);

        for (unsigned i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            // first half of fft input is last block
            _fftin[i * 2] = _lastBlockL[i]; // real part is left channel
            _fftin[i * 2 + 1] = _lastBlockR[i]; // imag part is right channel
            // second half of fft input is current block
            _fftin[(AUDIO_BLOCK_SAMPLES + i) * 2] = _floatBufL[i]; // real part is left channel
            _fftin[(AUDIO_BLOCK_SAMPLES + i) * 2 + 1] = _floatBufR[i]; // imag part is right channel
            // save for next time
            _lastBlockL[i] = _floatBufL[i];
            _lastBlockR[i] = _floatBufR[i];
        }

        // complex fft inplace [Re(0), Im(0), Re(1), Im(1) ... ]
        const static arm_cfft_instance_f32 *S;
        S = &arm_cfft_sR_f32_len256;
        arm_cfft_f32(S, _fftin, 0, 1);
        int buffidxMult = _buffIdx * AUDIO_BLOCK_SAMPLES * 4;
        _ptr_fftout_curr = _ptr_fftout + (buffidxMult);   // set pointer to proper segment of fftout array
        memcpy(_ptr_fftout_curr, _fftin, AUDIO_BLOCK_SAMPLES * 4 * sizeof(float32_t));  // copy samples from in to out at proper location

        // complex multiplication with frequency response
        _k = _buffIdx;
        memset(_acc, 0, sizeof(_acc));  // clear _acc array
        _kMult = _k * AUDIO_BLOCK_SAMPLES  * 4;
        _jMult = 0;
        for (unsigned j = 0; j < _nBlocks; j++) {
                _ptr_fftout_curr = _ptr_fftout + _kMult;
                _ptr_freqResponse_curr = _ptr_freqResponse + _jMult;
                // multiply  Y(z) = X(z)*H(z)
                arm_cmplx_mult_cmplx_f32(_ptr_fftout_curr, _ptr_freqResponse_curr, _acc2, 2*AUDIO_BLOCK_SAMPLES);
                // accumulate
                for (int q = 0; q < AUDIO_BLOCK_SAMPLES * 4; q=q+8) {
                    _acc[q] += _acc2[q];
                    _acc[q+1] += _acc2[q+1];
                    _acc[q+2] += _acc2[q+2];
                    _acc[q+3] += _acc2[q+3];
                    _acc[q+4] += _acc2[q+4];
                    _acc[q + 5] += _acc2[q + 5];
                    _acc[q + 6] += _acc2[q + 6];
                    _acc[q + 7] += _acc2[q + 7];
                }
            _k--;
            if (_k < 0)
            {
                _k = _nBlocks - 1;
            }
            _kMult = _k * AUDIO_BLOCK_SAMPLES * 4;
            _jMult += AUDIO_BLOCK_SAMPLES * 4;
        } // end np loop
        _buffIdx++;    
        _buffIdx = _buffIdx % _nBlocks;

        // complex ifft
        const static arm_cfft_instance_f32 *iS;
        iS = &arm_cfft_sR_f32_len256;
        arm_cfft_f32(iS, _acc, 1, 1);

        // overlap add
        for (unsigned i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            _floatBufL[i] = _acc[i * 2 + 0] * _gain;
            _floatBufR[i] = _acc[i * 2 + 1] * _gain;
        }
        bp = blockL->data;
        arm_float_to_q15(&_floatBufL[0], bp, AUDIO_BLOCK_SAMPLES);  
        bp = blockR->data;
        arm_float_to_q15(&_floatBufR[0], bp, AUDIO_BLOCK_SAMPLES);  
        transmit(blockL, 0);
        transmit(blockR, 1);
        release(blockL);
        release(blockR);
    }
}
