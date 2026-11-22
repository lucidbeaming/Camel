# Changelog

All notable changes to this project are documented here. The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [Unreleased]

### Changed
- Ported to the Mozzi 2 API: `Mozzi.h`, `MOZZI_CONTROL_RATE` / `MOZZI_AUDIO_RATE`, `ResonantFilter.h` with `setCutoffFreqAndResonance()`, `AudioOutput updateAudio()` returning `MonoOutput::from8Bit()`, and `mozziAnalogRead<10>()`. **Requires Mozzi 2.x.** The last version for Mozzi 1.x is tagged `mozzi-1.x` (#4)
