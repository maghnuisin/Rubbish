# PRD: Rubbish — Distortion / Saturation Effect (Version 2)

## Goal
A stereo distortion/saturation effect inspired by the warmth of tape-and-console saturators and the screaming resonant filters of the MS-20. Its signature: distortion that responds to how the sound is played. A producer should be able to add anything from subtle warmth to aggressive crunch, keep the punch, and keep the low end clean.

Version 2 adds **Twist**: three EQ bells that move by themselves after the distortion, each driven by its own LFO, so a held note keeps twisting and evolving (inspired by Leon Switch, "Midrange Bass With Ableton Stock Plugins").

## Features

### Drive panel
| Parameter | Range | Default | Notes |
|---|---|---|---|
| Drive | 0–36 dB | 6 dB | Input gain into the distortion stage |
| Push | Off / On | Off | Adds +20 dB extra drive |
| Character | Tape / Console / Tube / Synth | Tape | Dropdown menu. Tape = warm, soft clipping; Console = punchy, forward; Tube = thick, buzzy (asymmetric); Synth = raw, MS-20-style bite |
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

### Twist (new in version 2)
Global:
| Parameter | Range | Default | Notes |
|---|---|---|---|
| Twist | Off / On | Off | Off = sounds exactly like version 1 |
| Sync | Off / On | On | On = all three Speeds lock to the DAW tempo and song position |

Per bell (3 bells, each with its own colour; defaults listed as Bell 1 / Bell 2 / Bell 3):
| Parameter | Range | Default | Notes |
|---|---|---|---|
| Freq | 20 Hz–20 kHz | 200 Hz / 700 Hz / 2 kHz | Bell's home position |
| Gain | −24 to +24 dB | 0 / 0 / 0 dB | Boost or cut |
| Q | 0–100% | 37.5% / 37.5% / 37.5% | Bell width, shown as a percentage: 0% = Q 0.20 (widest), 100% = Q 1.00 (narrowest) |
| Shape | Smooth / Triangle / Ramp / Random wander | Smooth | LFO shape |
| Speed (Sync on) | 1/16T, 1/16, 1/16D, 1/8T, 1/8, 1/8D, 1/4T, 1/4, 1/4D, 1/2T, 1/2, 1/2D, 1, 2, 4, 8 bars | 1 bar / 2 bars / 1/2 | Shortest to longest. T = triplet (⅔ length), D = dotted (1½ × length) |
| Speed (Sync off) | 0.01–10 Hz | 0.5 / 0.3 / 0.8 Hz | One Speed knob shows whichever applies |
| Move | −100% to +100% | 0% / 0% / 0% | How far the LFO sweeps the bell's frequency; negative sweeps the opposite way; 0% = bell stays still |
| Start | 0–360° | 0° / 120° / 240° | LFO start phase |

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
- Signal flow: Clean Low split → distortion and filters (in the chosen Filter Order) → Twist → recombine low band → Dynamics Restore → Auto Gain → Mix → Output → safety limiter.
- Supports mono and stereo tracks; no other channel layouts.
- All parameters are smoothed so moving them never clicks or zips.
- All parameters are automatable by the DAW, and the full state is saved and restored with the DAW project.
- Distortion stage is oversampled internally to prevent aliasing (digital fizz). No user control. Latency is reported to the host.
- Safety limiter on the output: output can never exceed 0 dBFS, however high Drive, Push and Peak are set.- Filter resonance must never become unstable or blow up at any setting, including 100% Peak with Push on.
- Dynamics Restore and Filter Follow share one input-loudness tracker with a fixed, tuned reaction speed (no user control).
- Spread must stay mono-compatible: summing to mono must not cause noticeable cancellation or level loss.
- Clean Low uses a low-latency crossover; at 20 Hz the plugin behaves as if no split exists.
- Delta ignores Mix and Auto Gain, and plays at a level where the added content is clearly audible.
- Twist only processes the band above the Clean Low split, so Clean Low keeps protecting the low end.
- Each Twist LFO moves only its own bell's frequency; Gain and Q stay where they are set.
- With Sync on, LFO positions follow the DAW song position, so the same bar sounds the same on every playback and bounce (Random wander included).
- With Sync off, LFOs run freely from when playback starts.
- Twist shape changes are click-free: the Ramp's snap back and any jumps are smoothed.
- Switching Twist on or off never clicks.
- Twist volume jumps are handled by the existing Auto Gain and safety limiter (no extra Twist level control).
- Bells stay stable at every setting, including all three at +24 dB with Push on.
- Sound-quality acceptance: in blind, volume-matched listening tests on drums, bass, vocals and synth, each Character is judged musically usable, with no audible aliasing at any Drive setting.

## Interface Layout
- **Twist strip across the full width of the top of the window:**
  - Twist display fills the strip from left to right.
  - Display shows the three bells moving live in their colours, the combined EQ curve, and a faint bar under each bell showing how far it can roam.
  - Each bell has a coloured dot: drag left/right = Freq, drag up/down = Gain, scroll wheel = Q. Dots and knobs always stay in sync.
  - Under the display: Twist and Sync toggles on the left, then three rows of small controls, one row per bell (all visible at once, no tabs), each with the bell's coloured name and its Freq, Gain, Q, Speed, Move and Start knobs and Shape dropdown. Column names appear once, above the rows.
- Below the Twist strip, three compact panels, left to right, following signal flow: **Drive | Filters | Output**, each with its knobs in a single row where possible.
- Drive panel: Drive knob (biggest knob in the lower panels), Character dropdown, Push toggle, Clean Low small knob.
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
- 3-band Low/Mid/High split with per-band amounts and linear-phase option *(shelved for a later version)*
- Touch (louder input = more distortion) *(shelved for a later version)*
- A 4th Twist bell *(shelved for a later version)*
- A Twist "Tame" compressor knob *(shelved; revisit if loud sweeps sound squashed)*
- Unison (Voices, Detune, Width) for a Reese from a single sine or square *(shelved for a later version)*
- Twist LFOs moving Gain or Q; a separate Sync switch per bell; Square LFO shape
- Surprise me / randomise
- Filter-curve or other visual displays, except the Twist display
- Custom artwork or branded hardware look
- Preset browser or built-in preset library
- LFOs, step sequencers or any automatic modulation besides Filter Follow and the three Twist LFOs
- Sidechain input
- A/B comparison and in-plugin undo/redo
- MIDI learn
- Surround or multichannel layouts
- User-adjustable oversampling or reaction speeds
