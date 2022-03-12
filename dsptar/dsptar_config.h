#ifndef DSPTAR_CONFIG_H
#define DSPTAR_CONFIG_H

// ADC stuff to avoid magic numbers
#define ADC_BITS 10
#define ADC_MAX ((1 << ADC_BITS) - 1)

// Preamp gain control
#define PREAMP_GAIN_ADC_PIN A3  // analog input pin for preamp gain control
#define PREAMP_MIN_GAIN 5       // minimum gain value
#define PREAMP_MAX_GAIN 500     // maximum gain value

// Delay control
#define DELAY_MAX_SECS 1.1                    // maximum delay time in seconds
#define DELAY_NUM_TAPS 4                      // number of different delay taps
#define DELAY_DELAYMS {250, 500, 750, 1000}   // ms of delay on each tap (set a tap to 0 to disable)
#define DELAY_LOG2ATTENUATION {1, 2, 3, 4}    // log 2 of attenuation of each tap (i.e. 0->gain 1, 1->gain 1/2...)

// Output noise gate control
#define NOISE_GATE_THRESH 8192  // minimum sample absolute value to be sent to output

// Output volume control
#define VOLUME_ADC_PIN A1       // analog input pin for volume control
#define VOLUME_GAIN 0.25           // constant scale factor to apply to volume

// amount of audio memory, hopefully should not need to change
#define AUDIO_MEMORY_SIZE (DELAY_QUEUE_SIZE + 20)

#endif // DSPTAR_CONFIG_H
