/**
 * @file sine_volime.ino
 * @author Parthiv Krishna
 * @brief Test program to output a volume-controlled sine to the headphone jack
 * 
 * This program uses the Teensy Audio library to set up a sine wave generator
 * at a specified frequency, then connects the sine wave generator to the
 * audio shield output (headphones and line out). It controls the volume of
 * the output using the potentiometer on the audio shield. It prints the value
 * read from the volume ADC pin to the console.
 * 
 */

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// frequency of sine wave
#define FREQ 1000

#define VOLUME_ADC_PIN A1
// 10 bit ADC on volume knob
#define VOLUME_ADC_BITS 10
#define VOLUME_ADC_MAX ((1 << VOLUME_ADC_BITS) - 1)

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
    // Enable the audio shield
    audioShield.enable();
    sine.frequency(FREQ);
    Serial.println("setup complete. playing sine wave");
}

void loop() {
    // float so we can get a float when dividing
    float pot = analogRead(VOLUME_ADC_PIN);
    audioShield.volume(pot/VOLUME_ADC_MAX);
    Serial.println(pot);
}
