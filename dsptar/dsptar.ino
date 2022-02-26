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
// 10 bit ADC on volume knob
#define VOLUME_ADC_BITS 10
#define VOLUME_ADC_MAX ((1 << VOLUME_ADC_BITS) - 1)

// audio shield input
AudioInputI2S in;

Distortion dist;

// headphone/line out on audio shield 
AudioOutputI2S out;

// connect in to both ears (wired on right channel = 1)
AudioConnection inToDist(in, 1, dist, 0);
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
    Serial.println("setup complete");
    dist.setup(DISTORTION_ARR, DISTORTION_ARR_LEN);
}

void loop() {
    // float so we can get a float when dividing
    float pot = analogRead(VOLUME_ADC_PIN);
    audioShield.volume(pot/VOLUME_ADC_MAX);
}
