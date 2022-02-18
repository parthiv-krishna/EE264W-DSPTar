/**
 * @file passthrough.ino
 * @author Parthiv Krishna
 * @brief Test program to pass through audio input to output
 * 
 * This program uses the Teensy Audio library to pass line input
 * directly through to output; it also has volume control using
 * the potentiometer on the audio shield.
 * 
 */


#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#define VOLUME_ADC_PIN A1
// 10 bit ADC on volume knob
#define VOLUME_ADC_BITS 10
#define VOLUME_ADC_MAX ((1 << VOLUME_ADC_BITS) - 1)

// audio shield input
AudioInputI2S in;

// headphone/line out on audio shield 
AudioOutputI2S out;

// connect in to both ears (wired on right channel = 1)
AudioConnection inToRight(in, 1, out, 1);
AudioConnection inToLeft(in, 1, out, 0);

// audio shield control
AudioControlSGTL5000 audioShield;

void setup() {
    AudioMemory(4);
    // Enable the audio shield
    audioShield.enable();
    audioShield.inputSelect(AUDIO_INPUT_LINEIN); // line in input
    Serial.println("setup complete");
}

void loop() {
    // float so we can get a float when dividing
    float pot = analogRead(VOLUME_ADC_PIN);
    audioShield.volume(pot/VOLUME_ADC_MAX);
}
