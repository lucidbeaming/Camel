# Changelog

All notable changes to this project are documented here. The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [Unreleased]

## [1.0.0] - 2026-11-23

First tagged release of the Camel synth sketch, ported to the Mozzi 2 API. **Requires Mozzi 2.x**; the last version for Mozzi 1.x is tagged `mozzi-1.x`.

### Changed
- README rewritten: photo, video and recordings, wiring, install (clone into `camel-sample` so the Arduino tools can open it), a controls table and how the sound is built (#8)
- Relicensed from GPL-3.0 to MIT; earlier releases keep GPL-3.0 (#6)
- Ported to the Mozzi 2 API: `Mozzi.h`, `MOZZI_CONTROL_RATE` / `MOZZI_AUDIO_RATE`, `ResonantFilter.h` with `setCutoffFreqAndResonance()`, `AudioOutput updateAudio()` returning `MonoOutput::from8Bit()`, and `mozziAnalogRead<10>()`. **Requires Mozzi 2.x.** The last version for Mozzi 1.x is tagged `mozzi-1.x` (#4)

[Unreleased]: https://github.com/lucidbeaming/Camel/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/lucidbeaming/Camel/releases/tag/v1.0.0
