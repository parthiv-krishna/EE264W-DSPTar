/**
 * @file waveshape.ino
 * @author Parthiv Krishna
 * @brief Test program to test waveshape functionality.
 * 
 * This program uses the Teensy Audio library to pass line input
 * through a waveshaper; it also has volume control using
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

AudioEffectWaveshaper waveshape;

// headphone/line out on audio shield 
AudioOutputI2S out;

// connect in to both ears (wired on right channel = 1)
AudioConnection inToShape(in, 1, waveshape, 0);
AudioConnection shapeToRight(waveshape, 0, out, 1);
AudioConnection shapeToLeft(waveshape, 0, out, 0);

// audio shield control
AudioControlSGTL5000 audioShield;


float WAVESHAPE[17] = {
  -0.588,
  -0.579,
  -0.549,
  -0.488,
  -0.396,
  -0.320,
  -0.228,
  -0.122,
  0,
  0.122,
  0.228,
  0.320,
  0.396,
  0.488,
  0.549,
  0.579,
  0.588
};

void setup() {
    AudioMemory(40);
    // Enable the audio shield
    audioShield.enable();
    audioShield.inputSelect(AUDIO_INPUT_LINEIN); // line in input
    Serial.println("setup complete");
    waveshape.shape(WAVESHAPE, 17);
}

void loop() {
    // float so we can get a float when dividing
    float pot = analogRead(VOLUME_ADC_PIN);
    audioShield.volume(pot/VOLUME_ADC_MAX);
}
