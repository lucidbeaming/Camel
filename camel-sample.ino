#include <MozziGuts.h>
#include <IntMap.h>
#include <Oscil.h>
#include <Metronome.h>
#include <Ead.h>
#include <LowPassFilter.h>
#include <Phasor.h>
#include <Sample.h>
#include <RollingAverage.h>
#include <tables/sin2048_int8.h>
#include <tables/saw2048_int8.h>
#include <tables/cos2048_int8.h>
#include <tables/square_no_alias_2048_int8.h>
#include <samples/bamboo/bamboo_00_2048_int8.h>

#define CONTROL_RATE 64 // powers of 2 please

Metronome kMetro(25);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aLFO(SIN2048_DATA);
Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> aSaw(SAW2048_DATA);
Oscil <COS2048_NUM_CELLS, AUDIO_RATE> cosModulator(COS2048_DATA);
Oscil <SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> sqrModulator(SQUARE_NO_ALIAS_2048_DATA);
Sample <BAMBOO_00_2048_NUM_CELLS, AUDIO_RATE>aSample(BAMBOO_00_2048_DATA);
Ead envelope(CONTROL_RATE);
LowPassFilter lpf;
Phasor <AUDIO_RATE> kPhase;
RollingAverage <int, 64> kPitch;

const byte kSize = 5;
const byte modulation_ratio=3; // harmonics
const IntMap pitchMap(0,1024,40,5000);
const IntMap modMap(0,1024,10,700);
const IntMap byteMap(0,1024,0,255);
const IntMap bpm(0,1024,50,600);
byte pot[7] = { A0, A1, A2, A3, A4, A5 };
byte sw[4] = { 5, 6, 7 };
int knobVal, rate, cutoff, gain, pitch, chunk, upperA, upperB, upperC, lowerA, lowerB, lowerC, rollingC;
byte swA, swB, swC;
bool kMetrun, modulate, cut, phase;
long fm_intensity, modulation;
float pFreq = 200.f;
char wave, magnitude;

void setup() {
  startMozzi(CONTROL_RATE);
  for (byte i=0;i<=2;i++) {
    pinMode(sw[i], INPUT_PULLUP);
  }
  Serial.begin(9600);
  aSin.setFreq(440);
  aSaw.setFreq(440);
  kMetro.setBPM(50);
  kPhase.setFreq(pFreq);
  aLFO.setFreq(50);
  aSample.setFreq(10.24f);
  lpf.setResonance(50);
}

void updateControl(){
  swA = digitalRead(sw[0]);
  swB = digitalRead(sw[1]);
  swC = digitalRead(sw[2]);

  upperA = mozziAnalogRead(pot[0]);
  upperB = mozziAnalogRead(pot[1]);
  upperC = mozziAnalogRead(pot[2]);
  lowerA = mozziAnalogRead(pot[3]);
  lowerB = mozziAnalogRead(pot[4]);
  lowerC = mozziAnalogRead(pot[5]);

  rollingC = kPitch.next(lowerC);
  if (upperA > 5) {
    aLFO.setFreq(upperA*2);
    magnitude = aLFO.next();
    pitch = pitchMap(rollingC);
    pitch = ((((float)magnitude/127)*(pitch/2))+pitch);
  } else {
    pitch = pitchMap(rollingC);
  }
  
  if (swC) {
    if (swA) {
      aSin.setFreq(pitch);
    } else {
      aSaw.setFreq(pitch);
    }
  } else {
    aSample.setFreq((float)rollingC*.01f);
  }

  if (upperC <= 1020) {
    cutoff = byteMap(upperC);
    lpf.setCutoffFreq(cutoff);
    cut = true;
  } else {
    cut = false;
  }

  int modulation_freq = pitch * modulation_ratio;
  cosModulator.setFreq(modulation_freq);
  sqrModulator.setFreq(modulation_freq);
  fm_intensity = modMap(lowerA);

  if (lowerA <= 5) {
    modulate = false;
  } else {
    modulate = true;
  }

  if (upperB >= 5) {
    float pAmount = upperB * .0001f;
    float pFreq = pitch * (1 + pAmount);
    kPhase.setFreq(pFreq);
    phase = true;
  } else {
    phase = false;
  }
  
  rate = bpm(lowerB);
  kMetro.setBPM(rate);

  if (swC) {
    if (lowerB <= 1020) {
      if (!kMetrun) {
      kMetro.start();
      kMetrun = true;
      }
      if (kMetro.ready()) {
        envelope.start(30,700);
        aSample.start();
      }
      gain = (int) envelope.next();
    } else {
      if (kMetrun) {
        kMetro.stop();
        kMetrun = false;
      }
    }
  } else {
    if (!kMetrun) {
      kMetro.start();
      kMetrun = true;
    }
    if (kMetro.ready()) {
        aSample.start();
    }
  }

  
  
/*
  Serial.print("Switch 1: ");Serial.print(swA);
  Serial.print(" Switch 2: ");Serial.print(swB);
  Serial.print(" Switch 3: ");Serial.print(swC);
  Serial.print(" Upper 1: ");Serial.print(upperA);
  Serial.print(" Upper 2: ");Serial.print(upperB);
  Serial.print(" Upper 3: ");Serial.print(upperC);
  Serial.print(" Lower 1: ");Serial.print(lowerA);
  Serial.print(" Lower 2: ");Serial.print(lowerB);
  Serial.print(" Lower 3: ");Serial.print(lowerC);
  Serial.print(" mag: ");Serial.print(pitch);
  Serial.print("\n");
 */

}

int updateAudio(){
  
  if (swB) {
    modulation = fm_intensity * cosModulator.next();
  } else {
    modulation = fm_intensity * sqrModulator.next();
  }
  if (swC) {
    if (swA) {
        if (modulate==true) {
          chunk = aSin.phMod(modulation);
        } else {
          chunk = aSin.next();
        }
    } else {
      if (modulate==true) {
        chunk = aSaw.phMod(modulation);
      } else {
        chunk = aSaw.next();
      }
    }
    if (phase==true) {
      char asig = kPhase.next()>>24;
      chunk = ((int)asig-chunk)/2;
    }
    if (kMetrun==true) {
      if (cut==true) {
        wave = (char)lpf.next((gain * chunk)>>8);
      } else {
        wave = (gain * chunk)>>8;
      }
    } else {
      if (cut==true) {
        wave = (char)lpf.next(chunk);
      } else {
        wave = chunk;
      }
    }
  } else {
    if (modulate==true) {
      chunk = aSample.next();
      chunk = chunk + (modulation/2);
    } else {
      chunk = aSample.next();
    }
    if (phase==true) {
      char asig = kPhase.next()>>24;
      chunk = ((int)asig-chunk)/2;
    }
    if (cut==true) {
      wave = (char)lpf.next(chunk);
    } else {
      wave = chunk;
    }
  }

  //return (wave*magnitude)>>7;
  
  return wave;
}

void loop() {
  audioHook();
}
