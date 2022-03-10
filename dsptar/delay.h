#ifndef DELAY_H
#define DELAY_H

#include "Arduino.h"
#include "AudioStream.h"

#include "ringbuffer.h"

/**
 * @brief A struct to hold parameters for one delay tap
 * 
 */
typedef struct {
    int32_t delayBlocks;
    int8_t log2Attenuation;
} delay_tap_t;

/**
 * @brief An AudioStream object that applies delay
 *
 */
class Delay : public AudioStream {
public:
    /**
     * @brief Construct a new Delay object
     * 
     * Initializes the AudioStream super class to take in
     * one input block in inputQueueArray.
     * 
     */
    Delay(void) : AudioStream(1, _inputQueueArray), _delayQueue(nullptr), _delays(nullptr), _numTaps(0) {}
    
    /**
     * @brief Destroy the Delay object
     * 
     */
    ~Delay() {}

    void setup(float maxSecs, size_t numTaps);
    
    /**
     * @brief Configures a delay tap
     * 
     * Contribution from this tap will be the signal from delayMs ms
     * ago, scaled by 2^(-log2Attenuation)
     * 
     * @param index Index of delay tap (0 to MAX_DELAY_TAPS - 1)
     * @param delayMs Delay in ms
     * @param log2Attenuation log base 2 of attenuation (i.e. 0->gain 1, 1->gain 1/2...)
     */
    void setDelay(size_t index, int delayMs, int16_t log2Attenuation);

    /**
     * @brief Transmits an output block after applying delay
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
     * @brief ringbuffer queue of blocks
     * 
     */
    RingBuffer<audio_block_t*>* _delayQueue;

    /**
     * @brief delays for each tap in units of blocks
     * 
     * (~3ms resolution, good enough for our purposes)
     */
    delay_tap_t* _delays;

    size_t _numTaps;

};
#endif // DELAY_H
