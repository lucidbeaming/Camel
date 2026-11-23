# Camel

Arduino + Mozzi sketch for a keyless drone synthesizer.

![Camel: a worn Camel cigarette tin fitted with three toggle switches, three small knobs and three large white pointer knobs](https://res.cloudinary.com/lucidbeaming/image/upload/f_jpg,q_auto/c_fit,w_900/v1/lucidbeaming/soundart/camel-device)

- **Video:** [Camel Synth made with Mozzi audio library for Arduino](https://www.youtube.com/watch?v=uOU8HIqG_lI)
- **Recordings:** the [*Camel* tape](https://lucidbeaming.bandcamp.com/album/camel) on Bandcamp
- **Project page:** [lucidbeaming.com/art/sound-art](https://lucidbeaming.com/art/sound-art)

Camel is a synthesizer built into a former methamphetamine addict's stash box. It turns years of abuse into a squelching stream of lo-fi noise, played on its own or into an external mixer. It has no keyboard: six knobs and three switches shape a drone or a pulsing pattern, so it's played by turning, not by playing notes. This repo holds the Arduino sketch that runs it, written with the [Mozzi](https://github.com/sensorium/Mozzi) audio library.

## Requirements

- Any board Mozzi 2 supports, with six analog inputs and three spare digital pins. The sketch is board-agnostic (it compiles for the Uno and Nano, for example).
- [Mozzi](https://github.com/sensorium/Mozzi) **2.x**. The last version for Mozzi 1.x is tagged [`mozzi-1.x`](https://github.com/lucidbeaming/Camel/tree/mozzi-1.x).
- Six potentiometers (10 kΩ linear is typical), three on/off toggle switches, and an audio output (see [Wiring](#wiring))

### Wiring

| Arduino pin | Connects to |
|---|---|
| A0, A1, A2 | Upper row of knobs: wobble, phase detune, filter cutoff |
| A3, A4, A5 | Lower row of knobs: FM depth, tempo, pitch |
| D5, D6, D7 | Switches A, B, C, each between the pin and GND (the sketch uses the internal pull-ups) |
| Audio out | Mozzi's default output for your board: pin 9 on an Uno or Nano. Other boards use other pins; see [Mozzi's output modes](https://sensorium.github.io/Mozzi/). |

Each pot's wiper goes to its analog pin, with the outer legs on the board's supply (5 V or 3.3 V) and GND. On AVR boards like the Uno, the audio output is PWM, so it needs a simple RC low-pass filter before a jack or amplifier; the [Mozzi documentation](https://sensorium.github.io/Mozzi/) shows one.

## Install

1. Install Mozzi 2 from the Arduino IDE's Library Manager (search for "Mozzi"), or with `arduino-cli lib install Mozzi`.
2. Clone the repo into a folder named `camel-sample`. Arduino tools only open a sketch whose folder matches its `.ino` file name.

   ```bash
   git clone https://github.com/lucidbeaming/Camel.git camel-sample
   ```

3. Open `camel-sample/camel-sample.ino` in the Arduino IDE, select your board and upload. Or with `arduino-cli`, using your board's FQBN and port (both shown by `arduino-cli board list`). For an Uno:

   ```bash
   arduino-cli compile --fqbn arduino:avr:uno camel-sample
   arduino-cli upload  --fqbn arduino:avr:uno -p /dev/cu.usbmodem1101 camel-sample
   ```

## Controls

| Control | Pin | What it does |
|---|---|---|
| Upper knob 1 | A0 | **Wobble.** Near zero it's off. Turning it up starts an LFO (12–2,046 Hz) that swings the pitch by up to ±50 %. The LFO is read at the 64 Hz control rate, so above that the wobble turns into stepped, random-sounding jumps. |
| Upper knob 2 | A1 | **Phase detune.** Mixes in a ramp tuned up to 10 % above the pitch, for beating and growl. Off near zero. |
| Upper knob 3 | A2 | **Filter cutoff.** A resonant low-pass filter. Fully clockwise bypasses the filter. |
| Lower knob 1 | A3 | **FM depth.** Phase modulation by an oscillator at 3× the pitch. Off near zero. |
| Lower knob 2 | A4 | **Tempo**, 50–600 BPM. Each beat retriggers an envelope (30 ms attack, 700 ms decay) for a pulsing sound. Fully clockwise stops the pulse, leaving a continuous drone. |
| Lower knob 3 | A5 | **Pitch**, 40–5,000 Hz, smoothed so it glides. In sample mode it sets the sample's playback speed. |
| Switch A | D5 | **Waveform:** sine when the switch is open, sawtooth when it's closed. |
| Switch B | D6 | **FM shape:** cosine modulator when open, square when closed. |
| Switch C | D7 | **Source:** the oscillator when open; a short bamboo sample when closed, retriggered on every beat. |

"Open" means the switch isn't connecting the pin to GND.

## How it works

Mozzi runs two loops: `updateControl()` reads the knobs and switches 64 times a second, and `updateAudio()` computes each sample at Mozzi's default 16,384 Hz.

1. **Source:** a sine or saw oscillator at the pitch knob's frequency, or Mozzi's bamboo sample.
2. **FM:** when lower knob 1 is up, a cosine or square modulator at 3× the pitch phase-modulates the oscillator (or is mixed into the sample).
3. **Phase detune:** when upper knob 2 is up, a slightly sharp ramp is subtracted from the signal.
4. **Envelope:** in oscillator mode, the tempo knob's metronome retriggers an attack-decay envelope, unless the knob is fully clockwise.
5. **Filter:** a resonant low-pass with fixed resonance, unless upper knob 3 is fully clockwise.

The output is 8-bit, sent through Mozzi's `MonoOutput::from8Bit()`.

## Related projects

- [vid2midi](https://github.com/lucidbeaming/vid2midi): generate MIDI from the brightness of a video
- [pi-synths](https://github.com/lucidbeaming/pi-synths): scripts for building music machines on Raspberry Pi
- [p5-audio-visualizers](https://github.com/lucidbeaming/p5-audio-visualizers): fullscreen audio-reactive sketches for live visuals

## License

[MIT](LICENSE). Releases before this change were published under GPL-3.0, and those copies keep that license.
