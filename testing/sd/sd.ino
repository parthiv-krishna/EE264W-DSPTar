/**
 * @file sd.ino
 * @author Parthiv Krishna
 * @brief Test program to test SD card wav playback
 * 
 * This program uses the Teensy Audio library to play back a
 * WAV file from the SD card; it also has volume control using
 * the potentiometer on the audio shield.
 * 
 */

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#define FILENAME "SANDMAN.WAV"
#define VOLUME_ADC_PIN A1
// 10 bit ADC on volume knob
#define VOLUME_ADC_BITS 10
#define VOLUME_ADC_MAX ((1 << VOLUME_ADC_BITS) - 1)

AudioPlaySdWav           playWav1;
AudioOutputI2S           audioOutput;

AudioConnection          patchCord1(playWav1, 0, audioOutput, 0);
AudioConnection          patchCord2(playWav1, 1, audioOutput, 1);
AudioControlSGTL5000     sgtl5000_1;

// Teensy audio shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

void setup() {
  Serial.begin(115200);

  AudioMemory(8);

  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}

void playFile(const char *filename)
{
  Serial.print("Playing file: ");
  Serial.println(filename);

  playWav1.play(filename);

  // small delay to allow the file to start playing
  delay(25);

  while (playWav1.isPlaying()) {
     float vol = analogRead(VOLUME_ADC_PIN);
     vol = vol / VOLUME_ADC_MAX;
     sgtl5000_1.volume(vol);
  }
}


void loop() {
  playFile(FILENAME);  // filenames are always uppercase 8.3 format
  delay(1500);
}
