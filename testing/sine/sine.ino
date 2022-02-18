/**
 * @file sine.ino
 * @author Parthiv Krishna
 * @brief Test program to output a sine to the audio shield headphone jack
 * 
 * This program uses the Teensy Audio library to set up a sine wave generator
 * at a specified frequency, then connects the sine wave generator to the
 * audio shield output (headphones and line out).
 * 
 */

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// frequency of sine wave
#define FREQ 1000

// sine wave generator
AudioSynthWaveformSine sine;

// headphone/line out on audio shield 
AudioOutputI2S out;

// connect sine wave to both ears
AudioConnection patchCord1(sine, 0, out, 1);
AudioConnection patchCord2(sine, 0, out, 0);

// audio shield control
AudioControlSGTL5000 audioShield;

void setup() {

    AudioMemory(4);
    // Enable the audio shield and set the output volume.
    audioShield.enable();
    audioShield.volume(0.5);
    sine.frequency(FREQ);
    Serial.println("setup complete. playing sine wave");
}

void loop() {
    // intentionally blank
}
