#ifndef NOISEGATE_H
#define NOISEGATE_H

#include "Arduino.h"
#include "AudioStream.h"

#define DEFAULT_THRESH 0

/**
 * @brief An AudioStream object that applies a noise gate
 *
 */
class NoiseGate : public AudioStream {
public:
    /**
     * @brief Construct a new NoiseGate object
     * 
     * Initializes the AudioStream super class to take in
     * one input block in inputQueueArray.
     * 
     */
    NoiseGate(void) : AudioStream(1, _inputQueueArray), _thresh(DEFAULT_THRESH) {}
    
    /**
     * @brief Destroy the NoiseGate object
     * 
     */
    ~NoiseGate() {}
    
    /**
     * @brief Sets the threshold of the noise gate
     * 
     * @param thresh the threshold to use (must be between 0 and INT16_MAX, inclusive)
     */
    void setThresh(uint16_t thresh);

    /**
     * @brief Transmits an output block after applying noise gate
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
     * @brief The current threshold of the noise gate
     * 
     */
    float _thresh;
};

#endif // NOISEGATE_H
