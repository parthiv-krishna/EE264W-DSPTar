#ifndef DISTORTION_H
#define DISTORTION_H

#include "Arduino.h"
#include "AudioStream.h"

/**
 * @brief An AudioStream object that provides a distortion effect via waveshaping
 *
 */
class Distortion : public AudioStream {
public:
    /**
     * @brief Construct a new Distortion object
     * 
     * Initializes the AudioStream super class to take in
     * one input block in inputQueueArray.
     * 
     */
    Distortion(void) : AudioStream(1, _inputQueueArray), _distortionArr(nullptr) {}
    
    /**
     * @brief Destroy the Distortion object
     * 
     */
    ~Distortion();
    
    /**
     * @brief Sets up the Distortion object with a given array
     * 
     * @param distortionArr The array of waveshaping coefficients (see _distortion_arr)
     * @param length The length of distortion_arr (must be 2^n + 1 for 0<n<=15) 
     * @return true if the setup succeeded 
     * @return false otherwise
     */
    bool setup(int16_t *distortionArr, int length);

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
     * @brief The quantized waveshaping levels to use
     * 
     * These should be stored in Q.15 fixed point format. The first element corresponds
     * to the output level at input = -1. The last element corresponds to the output level
     * at input = 1. The values in between are used, along with linear interpolation, to
     * compute the output level at all input levels in between. To increase computation
     * speed, the number of elements must be a power of 2 plus 1 (i.e. 2^N + 1).
     * 
     */
    int16_t *_distortionArr;

    /**
     * @brief The amount of bits to shift in interpolation (see update)
     * 
     */
    int _bitShift;
};

#endif // DISTORTION_H
