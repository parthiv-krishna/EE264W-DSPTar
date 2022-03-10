#ifndef DSPTAR_CONFIG_H
#define DSPTAR_CONFIG_H

// ADC stuff to avoid magic numbers
#define ADC_BITS 10
#define ADC_MAX ((1 << ADC_BITS) - 1)

// Preamp gain control
#define PREAMP_GAIN_ADC_PIN A3  // analog input pin for preamp gain control
#define PREAMP_MIN_GAIN 50      // minimum gain value
#define PREAMP_MAX_GAIN 500     // maximum gain value

// Delay control
// testing: could handle up to 2s of delay
#define DELAY_MAX_SECS 2
#define DELAY_NUM_TAPS 4

// Output noise gate control
#define NOISE_GATE_THRESH 8192  // minimum sample absolute value to be sent to output

// Output volume control
#define VOLUME_ADC_PIN A1       // analog input pin for volume control
#define VOLUME_GAIN 1           // constant scale factor to apply to volume

// amount of audio memory, hopefully should not need to change
#define AUDIO_MEMORY_SIZE (DELAY_QUEUE_SIZE + 10)

#endif // DSPTAR_CONFIG_H
