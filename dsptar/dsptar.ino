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
#include "noisegate.h"

// audio shield input
AudioInputI2S in;

Preamp preamp;
Distortion dist;
NoiseGate gate;

// headphone/line out on audio shield 
AudioOutputI2S out;

// connect in to both ears (wired on right channel = 1)
AudioConnection inToAmp(in, 1, preamp, 0);
AudioConnection ampToDist(preamp, 0, dist, 0);
AudioConnection distToGate(dist, 0, gate, 0);
AudioConnection gateToRight(gate, 0, out, 1);
AudioConnection gateToLeft(gate, 0, out, 0);

// audio shield control
AudioControlSGTL5000 audioShield;

void setup() {
    Serial.begin(115200);
    AudioMemory(40);
    // Enable the audio shield
    audioShield.enable();
    audioShield.inputSelect(AUDIO_INPUT_LINEIN); // line in input
    dist.setup(DISTORTION_ARR, DISTORTION_ARR_LEN);
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
