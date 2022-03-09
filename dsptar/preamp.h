#ifndef PREAMP_H
#define PREAMP_H

#include "Arduino.h"
#include "AudioStream.h"

#define DEFAULT_GAIN 1.0

/**
 * @brief An AudioStream object that applies preamplification on raw samples
 *
 */
class Preamp : public AudioStream {
public:
    /**
     * @brief Construct a new Preamp object
     * 
     * Initializes the AudioStream super class to take in
     * one input block in inputQueueArray.
     * 
     */
    Preamp(void) : AudioStream(1, _inputQueueArray), _gain(DEFAULT_GAIN) {}
    
    /**
     * @brief Destroy the Preamp object
     * 
     */
    ~Preamp() {}
    
    /**
     * @brief Sets the gain of the preamp
     * 
     * @param gain the gain to use
     */
    void setGain(float gain);

    /**
     * @brief Transmits an output block after applying gain
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
     * @brief The current gain of the preamp
     * 
     */
    float _gain;
};

#endif // PREAMP_H
