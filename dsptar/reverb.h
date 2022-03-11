#ifndef REVERB_H
#define REVERB_H

#include "Arduino.h"
#include "AudioStream.h"
#include "arm_math.h"


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
     * one input block in inputQueueArray.
     * 
     */
    Reverb(void) : AudioStream(1, _inputQueueArray), _irArr(nullptr) {}
    
    /**
     * @brief Destroy the Reverb object
     * 
     */
    ~Reverb();
    
    /**
     * @brief Sets up the Reverb object with a given impulse response
     * 
     * @param impulseResponse The array of impulse response (see _irArr)
     * @param length The length of impulseResponse (must be 2^n for 0<n<=15)
     * @return true if the setup succeeded
     * @return false otherwise
     */
    bool setup(int16_t *impulseResponse, size_t length);

    /**
     * @brief Transmits an output block after applying distortion 
     * 
     */
    virtual void update(void);

private:
    /**
     * @brief The input block array (just one input)
     * 
     */
    audio_block_t *_inputQueueArray[1];
    
    /**
     * @brief The impulse response to use
     * 
     * These should be stored in Q.15 fixed point format. The first element corresponds
     * to the output level at input = -1. The last element corresponds to the output level
     * at input = 1. The values in between are used, along with linear interpolation, to
     * compute the output level at all input levels in between. To increase computation
     * speed, the number of elements must be a power of 2 plus 1 (i.e. 2^N + 1).
     * 
     */
    int16_t *_irArr;

    /**
     * @brief The length of the _irArr
     * 
     */
    size_t _irArrLen;

    /**
     * @brief Data for the DSP accelerator to use
     * 
     */
    arm_fir_instance_q15 _fir;

    q15_t *_state;

};



#endif // REVERB_H