/**
 * @file dsptar.ino
 * @author Parthiv Krishna
 * @brief Main DSPtar arduino script
 * 
 */


#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "dsptar_config.h"
#include "preamp.h"
#include "distortion.h"
#include "distortion_array.h"
#include "delay.h"
#include "noisegate.h"

// audio shield input
AudioInputI2S in;

Preamp preamp;
Distortion dist;
Delay dly;
NoiseGate gate;


// headphone/line out on audio shield 
AudioOutputI2S out;

// connect in to both ears (wired on right channel = 1)
AudioConnection inToAmp(in, 1, preamp, 0);
AudioConnection ampToDist(preamp, 0, dist, 0);
AudioConnection distToDelay(dist, 0, dly, 0);
AudioConnection delayToGate(dly, 0, gate, 0);
AudioConnection gateToRight(gate, 0, out, 1);
AudioConnection gateToLeft(gate, 0, out, 0);

// audio shield control
AudioControlSGTL5000 audioShield;

void setup() {
    Serial.begin(115200);
    AudioMemory(AUDIO_MEMORY_SIZE);
    // Enable the audio shield
    audioShield.enable();
    audioShield.inputSelect(AUDIO_INPUT_LINEIN); // line in input
    
    dist.setup(DISTORTION_ARR, DISTORTION_ARR_LEN);
    dly.setup(DELAY_MAX_SECS, DELAY_NUM_TAPS);

    int16_t delayMs[DELAY_NUM_TAPS] = DELAY_DELAYMS;
    int16_t delayAtten[DELAY_NUM_TAPS]  = DELAY_LOG2ATTENUATION;
    for (size_t i = 0; i < DELAY_NUM_TAPS; i++) {
        dly.setDelay(i, delayMs[i], delayAtten[i]);
    }

    gate.setThresh(NOISE_GATE_THRESH);
    
    Serial.println("setup complete");
}

void loop() {

    // update preamp gain
    float gainPot = analogRead(PREAMP_GAIN_ADC_PIN);
    // linear interpolation
    float gain = PREAMP_MIN_GAIN + gainPot * (PREAMP_MAX_GAIN - PREAMP_MIN_GAIN) / (float) ADC_MAX;
    preamp.setGain(gain);

    // update output volume
    float volPot = analogRead(VOLUME_ADC_PIN);
    audioShield.volume((volPot * VOLUME_GAIN)/ADC_MAX);

}
