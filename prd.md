# PRD: Rubbish — Distortion / Saturation Effect (Version 1)

## Goal
A stereo distortion/saturation effect inspired by the warmth of tape-and-console saturators and the screaming resonant filters of the MS-20. Its signature: distortion that responds to how the sound is played. A producer should be able to add anything from subtle warmth to aggressive crunch, keep the punch, and keep the low end clean.

## Features

### Drive panel
| Parameter | Range | Default | Notes |
|---|---|---|---|
| Drive | 0–36 dB | 6 dB | Input gain into the distortion stage |
| Push | Off / On | Off | Adds +20 dB extra drive |
| Character | Tape / Console / Tube / Synth | Tape | Tape = warm, soft clipping; Console = punchy, forward; Tube = thick, buzzy (asymmetric); Synth = raw, MS-20-style bite |
| Clean Low | 20–500 Hz | 20 Hz (effectively off) | Everything below this frequency bypasses the distortion and is recombined cleanly |

### Filters panel
| Parameter | Range | Default | Notes |
|---|---|---|---|
| Low Cut | 20 Hz–2 kHz | 20 Hz | Resonant high-pass filter |
| Low Peak | 0–100% | 0% | Resonance of Low Cut; low values give a bass bump, high values howl |
| High Cut | 200 Hz–20 kHz | 20 kHz | Resonant low-pass filter |
| High Peak | 0–100% | 0% | Resonance of High Cut; can scream at high settings |
| Filter Order | Clean first / Scream | Clean first | Clean first = distortion → filters; Scream = filters → distortion |
| Filter Follow | −100% to +100% | 0% | Input loudness moves the High Cut: positive opens it on hits, negative closes it |

### Output panel
| Parameter | Range | Default | Notes |
|---|---|---|---|
| Dynamics Restore | 0–100% | 0% | Reapplies the clean input's moment-to-moment volume shape to the distorted signal |
| Spread | 0–100% | 0% | Distorts the stereo sides differently from the centre to add width |
| Auto Gain | Off / On | On | Keeps average output loudness close to the input's as Drive changes |
| Mix | 0–100% | 100% | Dry/wet blend |
| Output | −24 to +12 dB | 0 dB | Final level |
| Delta | Off / On | Off | Outputs only what the plugin adds (processed minus dry) |

## Behaviour
- Signal flow: Clean Low split → distortion and filters (in the chosen Filter Order) → recombine low band → Dynamics Restore → Auto Gain → Mix → Output → safety limiter.
- Supports mono and stereo tracks; no other channel layouts.
- All parameters are smoothed so moving them never clicks or zips.
- All parameters are automatable by the DAW, and the full state is saved and restored with the DAW project.
- Distortion stage is oversampled internally to prevent aliasing (digital fizz). No user control. Latency is reported to the host.
- Safety limiter on the output: output can never exceed 0 dBFS, however high Drive, Push and Peak are set.- Filter resonance must never become unstable or blow up at any setting, including 100% Peak with Push on.
- Dynamics Restore and Filter Follow share one input-loudness tracker with a fixed, tuned reaction speed (no user control).
- Spread must stay mono-compatible: summing to mono must not cause noticeable cancellation or level loss.
- Clean Low uses a low-latency crossover; at 20 Hz the plugin behaves as if no split exists.
- Delta ignores Mix and Auto Gain, and plays at a level where the added content is clearly audible.
- Sound-quality acceptance: in blind, volume-matched listening tests on drums, bass, vocals and synth, each Character is judged musically usable, with no audible aliasing at any Drive setting.

## Interface Layout
- Three panels, left to right, following signal flow: **Drive | Filters | Output**.
- Drive panel: large Drive knob (biggest control on screen), Push toggle beneath it, Character as four labelled buttons (2×2), Clean Low small knob.
- Filters panel: Low Cut knob with its Peak knob beside it; High Cut knob with its Peak knob beside it; Filter Follow knob.
- Output panel (far right): Dynamics Restore, Spread, Mix and Output knobs, Auto Gain and Delta toggles, vertical output level meter.
- Vertical input level meter on the far left, mirroring the output meter on the right.
- Both meters show the current peak level as a number in dB at the top of the meter.
- Filter Order is a swap button (↔ icon) placed between the Drive and Filters panels. Clicking it toggles Clean first / Scream, and the two panels swap places so the one on the left is processed first (Scream shows Filters before Drive). Hovering the button shows the current order.
- Knobs: vertical drag; Shift/Cmd/Ctrl-drag for fine adjustment; double-click resets to default; scroll wheel nudges; exact value with unit shown on hover/drag; click the value to type one.
- No bypass button (the DAW provides it).
- Resizable window; all text readable on a laptop screen.
- Plugin name everywhere (DAW plugin list, window title, code, files and bundle IDs): `Rubbish`.
- UI scope: standard framework-style knobs and buttons in this layout. No custom artwork.

## Out of Scope (for now)
- 3-band Low/Mid/High split with per-band amounts and linear-phase option *(planned for version 2)*
- Touch (louder input = more distortion) *(planned for version 2)*
- Surprise me / randomise
- Filter-curve or other visual displays
- Custom artwork or branded hardware look
- Preset browser or built-in preset library
- LFOs, step sequencers or any automatic modulation besides Filter Follow
- Sidechain input
- A/B comparison and in-plugin undo/redo
- MIDI learn
- Surround or multichannel layouts
- User-adjustable oversampling or reaction speeds
