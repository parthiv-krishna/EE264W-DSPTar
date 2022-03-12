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
#include "eq.h"
#include "reverb.h"
#include "reverb_array.h"
#include "noisegate.h"

// audio shield input
AudioInputI2S in;

Preamp preamp;
Distortion dist;
Delay dly;
NoiseGate gate;
EQ eq;
Reverb verb;
NoiseGate outGateR;
NoiseGate outGateL;

AudioPlaySdWav wav;
AudioMixer4 mixerL;
AudioMixer4 mixerR;

// headphone/line out on audio shield
AudioOutputI2S out;

// connect in to both ears (wired on right channel = 1)
AudioConnection inToAmp(in, 1, preamp, 0);
AudioConnection ampToDist(preamp, 0, dist, 0);
AudioConnection distToDelay(dist, 0, dly, 0);
AudioConnection delayToGate(dly, 0, gate, 0);
AudioConnection gateToEq(gate, 0, eq, 0);
AudioConnection eqToVerbL(eq, 0, verb, 0);
AudioConnection eqToVerbR(eq, 0, verb, 1);
AudioConnection verbToGateL(verb, 0, outGateL, 0);
AudioConnection verbToGateR(verb, 1, outGateR, 0);

#if PLAY_WAV
AudioConnection gateToMixL(outGateL, 0, mixerL, 0);
AudioConnection gateToMixR(outGateR, 0, mixerR, 0);
AudioConnection wavToMixL(wav, 0, mixerL, 1);
AudioConnection wavToMixR(wav, 1, mixerR, 1);
AudioConnection mixLToOut(mixerL, 0, out, 0);
AudioConnection mixRToOut(mixerR, 0, out, 1);
#else // !PLAY_WAV
AudioConnection gateToOutL(outGateL, 0, out, 0);
AudioConnection gateToOutR(outGateR, 0, out, 1);
#endif // PLAY_WAV

// audio shield control
AudioControlSGTL5000 audioShield;

// buffers needed for storing reverb FFT (defined statically)
// stored in DMA memory region. FFT length is AUDIO_BLOCK_SAMPLES * 2 (extra *2 since complex)
float32_t DMAMEM reverbFftTemp[AUDIO_BLOCK_SAMPLES * 2 * 2];
float32_t DMAMEM reverbFftCoeffs[REVERB_ARR_LEN / AUDIO_BLOCK_SAMPLES][AUDIO_BLOCK_SAMPLES * 2 * 2];

void setup()
{
    bool success = true;

    Serial.begin(115200);
    AudioMemory(AUDIO_MEMORY_SIZE);
    // Enable the audio shield
    audioShield.enable();
    audioShield.inputSelect(AUDIO_INPUT_LINEIN); // line in input

    success &= dist.setup(DISTORTION_ARR, DISTORTION_ARR_LEN);

    dly.setup(DELAY_MAX_SECS, DELAY_NUM_TAPS);
    int16_t delayMs[DELAY_NUM_TAPS] = DELAY_DELAYMS;
    int16_t delayAtten[DELAY_NUM_TAPS] = DELAY_LOG2ATTENUATION;
    for (size_t i = 0; i < DELAY_NUM_TAPS; i++)
    {
        dly.setDelay(i, delayMs[i], delayAtten[i]);
    }

    gate.setThresh(DELAY_NOISE_GATE_THRESH);

    eq.setLowpass(0, 800);

    success &= verb.setup(REVERB_GAIN, *reverbFftCoeffs, REVERB_ARR, REVERB_ARR_LEN, reverbFftTemp);

    outGateL.setThresh(OUTPUT_NOISE_GATE_THRESH);
    outGateR.setThresh(OUTPUT_NOISE_GATE_THRESH);

    mixerL.gain(0, MIXER_GUITAR_GAIN);
    mixerR.gain(0, MIXER_GUITAR_GAIN);
    mixerL.gain(1, MIXER_WAV_GAIN);
    mixerR.gain(1, MIXER_WAV_GAIN);

    if (success)
    {
        Serial.println("setup complete");
    }
    else
    {
        Serial.println("setup failed");
    }

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

void loop()
{

#if PLAY_WAV
    Serial.print("Playing file: ");
    Serial.println(FILENAME);

    wav.play(FILENAME);

    // small delay to allow the file to start playing
    delay(25);

    while (wav.isPlaying()) {
#endif // PLAY_WAV
        // update preamp gain
        float gainPot = analogRead(PREAMP_GAIN_ADC_PIN);
        // linear interpolation
        float gain = PREAMP_MIN_GAIN + gainPot * (PREAMP_MAX_GAIN - PREAMP_MIN_GAIN) / (float)ADC_MAX;
        preamp.setGain(gain);

        // update eq lowpass cutoff
        float freqPot = analogRead(EQ_LOWPASS_FREQ_ADC_PIN);
        // linear interpolation
        float freq = EQ_MIN_LOWPASS_FREQ + freqPot * (EQ_MAX_LOWPASS_FREQ - EQ_MIN_LOWPASS_FREQ) / (float)ADC_MAX;
        eq.setLowpass(0, freq);

        // update output volume
        float volPot = analogRead(VOLUME_ADC_PIN);
        float vol = (volPot * VOLUME_GAIN) / ADC_MAX;
        audioShield.volume(vol);

        char buf[128];
        snprintf(buf, sizeof(buf), "Gain: %d, Freq: %d, Volume %d", (int)gain, (int)freq, (int)vol);
        Serial.println(buf);

#if PLAY_WAV
    }

    delay(1500);
#endif // PLAY_WAV
}
