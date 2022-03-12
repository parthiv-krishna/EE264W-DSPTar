#ifndef EQ_H
#define EQ_H

#include "Arduino.h"
#include "AudioStream.h"

#define SQRT2_OVER_2 0.7071f
#define TWO_TO_THE_30 1073741824.0f
#define PI 3.141592654f

class EQ : public AudioStream
{
public:
    /**
     * @brief Construct a new EQ object
     * 
     * Initializes the AudioStream super class to take in
     * an input block in inputQueueArray.
     * 
     */
    EQ(void) : AudioStream(1, _inputQueueArray) {
        // by default, the filter will not pass anything
        for (int i=0; i<32; i++) _coeffs[i] = 0;
    }

    /**
     * @brief Set the coefficients to given Q.30 values
     * 
     * @param stage The filter within the cascade to set
     * @param coefficients The coefficients [b0, b1, b2, a1, a2]
     */
    void setCoefficients(uint32_t stage, const int *coefficients);

    /**
     * @brief Set the coefficients to given float values
     * 
     * @param stage The filter within the cascade to set
     * @param coefficients The coefficients [b0, b1, b2, a1, a2]
     */
    void setCoefficients(uint32_t stage, const double *coefficients);

    /**
     * @brief Set the coefficients to a lowpass filter
     * 
     * @param stage The filter within the cascade to set
     * @param frequency The cutoff frequency of the lowpass
     * @param q The q factor of the lowpass (default SQRT2_OVER_2)
     */
    void setLowpass(uint32_t stage, float frequency, float q = SQRT2_OVER_2);

    /**
     * @brief Set the coefficients to a highpass filter
     * 
     * @param stage The filter within the cascade to set
     * @param frequency The cutoff frequency of the highpass
     * @param q The q factor of the highpass (default SQRT2_OVER_2)
     */
    void setHighpass(uint32_t stage, float frequency, float q = SQRT2_OVER_2);

    /**
     * @brief Set the coefficients to a bandpass filter
     * 
     * @param stage The filter within the cascade to set
     * @param frequency The center frequency of the bandpass
     * @param q The q factor of the bandpass (default 1.0)
     */
    void setBandpass(uint32_t stage, float frequency, float q = 1.0);

    /**
     * @brief Set the coefficients to a notch filter
     * 
     * @param stage The filter within the cascade to set
     * @param frequency The center frequency of the notch
     * @param q The q factor of the notch (default SQRT2_OVER_2)
     */
    void setNotch(uint32_t stage, float frequency, float q = 1.0);

    /**
     * @brief Set the coefficients to a low shelf filter
     * 
     * @param stage The filter within the cascade to set
     * @param frequency The center frequency of the shelf
     * @param gain The gain of the shelf plateu
     * @param slope The slope of the shelf ledge
     */
    void setLowShelf(uint32_t stage, float frequency, float gain, float slope = 1.0f);

    /**
     * @brief Set the coefficients to a high shelf filter
     * 
     * @param stage The filter within the cascade to set
     * @param frequency The center frequency of the shelf
     * @param gain The gain of the shelf plateu
     * @param slope The slope of the shelf ledge
     */
    void setHighShelf(uint32_t stage, float frequency, float gain, float slope = 1.0f);

    /**
     * @brief Transmits an output blocks after applying EQ 
     * 
     */
    virtual void update(void);

private:
    int32_t _coeffs[32];  // up to 4 cascaded biquads
    audio_block_t *_inputQueueArray[1];

    bool _setup;
};

#endif // EQ_H
