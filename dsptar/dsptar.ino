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

#include "distortion.h"
#include "distortion_array.h"

#define VOLUME_ADC_PIN A1
#define INPUT_GAIN_ADC_PIN A3
// 10 bit ADC on volume knob
#define ADC_BITS 10
#define ADC_MAX ((1 << ADC_BITS) - 1)
#define MIN_GAIN 50.0
#define MAX_GAIN 500.0

// audio shield input
AudioInputI2S in;

AudioAmplifier inputAmp;

Distortion dist;

// headphone/line out on audio shield 
AudioOutputI2S out;

// connect in to both ears (wired on right channel = 1)
AudioConnection inToAmp(in, 1, inputAmp, 0);
AudioConnection ampToDist(inputAmp, 0, dist, 0);
AudioConnection distToRight(dist, 0, out, 1);
AudioConnection distToLeft(dist, 0, out, 0);

// audio shield control
AudioControlSGTL5000 audioShield;

void setup() {
    Serial.begin(115200);
    AudioMemory(40);
    // Enable the audio shield
    audioShield.enable();
    audioShield.inputSelect(AUDIO_INPUT_LINEIN); // line in input
    dist.setup(DISTORTION_ARR, DISTORTION_ARR_LEN);
    Serial.println("setup complete");
}

void loop() {
    // float so we can get a float when dividing
    float volPot = analogRead(VOLUME_ADC_PIN);
    audioShield.volume(volPot/ADC_MAX);
    
    float gainPot = analogRead(INPUT_GAIN_ADC_PIN);
    float gain = MIN_GAIN + gainPot * (MAX_GAIN - MIN_GAIN) / ADC_MAX;
    inputAmp.gain(gain);
}
