# PRD: Rubbish — Distortion / Saturation Effect (Version 3)

## Goal
A stereo distortion/saturation effect inspired by the warmth of tape and tube saturators and the screaming resonant filters of the MS-20. Its signature: distortion that responds to how the sound is played. A producer should be able to add anything from subtle warmth to aggressive crunch, keep the punch, and keep the low end clean.

Version 2 adds **Twist**: three EQ bells that move by themselves after the distortion, each driven by its own LFO, so a held note keeps twisting and evolving (inspired by Leon Switch, "Midrange Bass With Ableton Stock Plugins").

Version 3 moves the movement into its own **LFO section**: three LFOs, all visible at once, that can be dragged onto any knob in Rubbish (Serum-style, simplified), with a coloured ring on the knob to set how much it moves. Each LFO gets a Curve so it can rise faster than it falls or vice versa, plus Smooth, Jitter, Offset and Unipolar.

## Features

### Drive panel
| Parameter | Range | Default | Notes |
|---|---|---|---|
| Drive | 0–36 dB | 6 dB | Input gain into the distortion stage |
| Push | Off / On | Off | Makes the distortion hotter *and* harder (see Push behaviour below) |
| Character | Tape / Tube / Heavy / Edge / Grit | Tape | Dropdown menu. See Characters below |
| Clean Low | 20–500 Hz | 20 Hz (effectively off) | Everything below this frequency bypasses the distortion and is recombined cleanly |

### Characters
Each Character is modelled on one of Soundtoys Decapitator's styles (measurements in `decapitator-analysis.md`) and changes character as Drive is turned, not just level.

| Character | Based on | Sound | How it responds as Drive goes up |
|---|---|---|---|
| Tape | A | Warm, soft clipping | Starts with gentle 2nd-harmonic warmth, moves towards odd harmonics as it's pushed, and levels off at a ceiling (squashes like tape rather than getting endlessly dirtier) |
| Tube | T | Thick, buzzy (asymmetric) | Starts balanced; the asymmetry grows with Drive, so it gets thicker and more even-harmonic the harder it's pushed |
| Heavy | E | Big, weighty, even-harmonic | Clean at low Drive, then rises steeply. Dominated by the 2nd harmonic (adds weight to bass). No ceiling: keeps getting dirtier all the way up |
| Edge | N | Clean, then bright and biting | Almost spotless through the lower third of Drive, then breaks up sharply. Odd-harmonic at every setting, with a long bright upper-harmonic tail |
| Grit | P | Dirty from the start, buzzy | Already adds colour at Drive 0. Even-harmonic and tube-like, but brighter than Tube and the most distortion of any Character without Push |

### Push behaviour
Modelled on Decapitator's Punish. Push is not just extra gain:
- **Hotter:** adds +20 dB extra drive. With Push on, Drive at minimum sounds about as distorted as Drive at its midpoint with Push off.
- **Harder:** shifts every Character towards more odd harmonics and a stronger upper-harmonic tail, without changing its basic personality.
- **Character-specific top end:** Tape's ceiling rises slightly but still levels off; Heavy and Edge keep getting dirtier to maximum Drive; Tube and Grit **fold over** (distortion peaks around 60–70% of Drive, then eases off at the top).

### Filters panel
| Parameter | Range | Default | Notes |
|---|---|---|---|
| Low Cut | 20 Hz–2 kHz | 20 Hz | Resonant high-pass filter |
| Low Peak | 0–100% | 0% | Resonance of Low Cut; low values give a bass bump, high values howl |
| High Cut | 200 Hz–20 kHz | 20 kHz | Resonant low-pass filter |
| High Peak | 0–100% | 0% | Resonance of High Cut; can scream at high settings |
| Filter Order | Clean first / Scream | Clean first | Clean first = distortion → filters; Scream = filters → distortion |
| Filter Follow | −100% to +100% | 0% | Input loudness moves the High Cut: positive opens it on hits, negative closes it |

### Twist (new in version 2, simplified in version 3)
| Parameter | Range | Default | Notes |
|---|---|---|---|
| Twist | Off / On | Off | Off = the three bells are bypassed |

Per bell (3 bells, each with its own colour; defaults listed as Bell 1 / Bell 2 / Bell 3):
| Parameter | Range | Default | Notes |
|---|---|---|---|
| Freq | 20 Hz–20 kHz | 200 Hz / 700 Hz / 2 kHz | Bell's home position |
| Gain | −24 to +24 dB | 0 / 0 / 0 dB | Boost or cut |
| Q | 0–100% | 37.5% / 37.5% / 37.5% | Bell width, shown as a percentage: 0% = Q 0.20 (widest), 100% = Q 1.00 (narrowest) |

Each bell's Freq comes with its matching LFO already connected (LFO 1 → Bell 1 Freq, LFO 2 → Bell 2 Freq, LFO 3 → Bell 3 Freq) at 0%, so nothing moves until the ring is dragged. Version 2's per-bell Move, Shape, Speed, Start and shared Sync are replaced by the LFO section.

### LFO section (new in version 3)
Per LFO (3 LFOs, coloured to match Bells 1, 2 and 3; defaults listed as LFO 1 / LFO 2 / LFO 3):
| Parameter | Range | Default | Notes |
|---|---|---|---|
| Shape | Smooth / Triangle / Ramp / Random wander | Smooth / Smooth / Smooth | LFO shape |
| Curve | −100% to +100% | 0% | 0% = even; positive = rises fast, falls slowly; negative = rises slowly, falls fast |
| Sync | Off / On | On | Each LFO has its own Sync. On = Speed locks to the DAW tempo and song position |
| Speed (Sync on) | 1/16T, 1/16, 1/16D, 1/8T, 1/8, 1/8D, 1/4T, 1/4, 1/4D, 1/2T, 1/2, 1/2D, 1, 2, 4, 8 bars | 1 bar / 2 bars / 1/2 | Shortest to longest. T = triplet (⅔ length), D = dotted (1½ × length) |
| Speed (Sync off) | 0.01–10 Hz | 0.5 / 0.3 / 0.8 Hz | One Speed knob shows whichever applies |
| Start | 0–360° | 0° / 120° / 240° | LFO start phase |
| Smooth | 0–100% | 0% | Rounds off sharp corners and jumps so they glide |
| Jitter | 0–100% | 0% | Adds randomness so the movement never repeats exactly |
| Offset | −100% to +100% | 0% | Shifts the middle of the wobble up or down |
| Unipolar | Off / On | Off | On = sweeps only one way from the knob's position |

Connections (the coloured rings):
| Parameter | Range | Default | Notes |
|---|---|---|---|
| Amount (one per LFO per knob) | −100% to +100% | 0% | How far that LFO moves that knob; negative sweeps the opposite way. On a bell's Freq, 100% = ±3 octaves (same as version 2's Move) |

- Knobs that can take an LFO (20): each bell's Freq, Gain and Q; Drive; Clean Low; Low Cut; Low Peak; High Cut; High Peak; Filter Follow; Dynamics Restore; Spread; Mix; Output.
- No connection limit: every LFO can connect to all 20 knobs at once, and one knob can have rings from all three LFOs.
- Switches and dropdowns (Push, Character, Twist, Sync, Unipolar, Auto Gain, Delta, Filter Order) and the LFOs' own controls cannot take an LFO.

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
- Signal flow: Clean Low split → distortion and filters (in the chosen Filter Order) → Twist → recombine low band → Dynamics Restore → Auto Gain → Mix → Output → safety limiter. The LFOs move knobs anywhere in this chain; the safety limiter stays last.
- Supports mono and stereo tracks; no other channel layouts.
- All parameters are smoothed so moving them never clicks or zips.
- All parameters are automatable by the DAW, and the full state is saved and restored with the DAW project.
- Distortion stage is oversampled internally to prevent aliasing (digital fizz). No user control. Latency is reported to the host.
- Safety limiter on the output: output can never exceed 0 dBFS, however high Drive, Push and Peak are set.
- Filter resonance must never become unstable or blow up at any setting, including 100% Peak with Push on.
- Dynamics Restore and Filter Follow share one input-loudness tracker with a fixed, tuned reaction speed (no user control).
- Spread must stay mono-compatible: summing to mono must not cause noticeable cancellation or level loss.
- Clean Low uses a low-latency crossover; at 20 Hz the plugin behaves as if no split exists.
- Delta ignores Mix and Auto Gain, and plays at a level where the added content is clearly audible.
- Twist only processes the band above the Clean Low split, so Clean Low keeps protecting the low end.
- An LFO can never push a knob past that knob's own range.
- With all ring amounts at 0%, Rubbish sounds exactly like version 2 with Move at 0%.
- With Twist off, LFOs connected to other knobs (e.g. Drive) keep moving them; only the bells are bypassed.
- Curve bends Smooth, Triangle and Ramp; Random wander ignores Curve.
- Unipolar with a positive amount sweeps up from the knob's position; with a negative amount, down.
- Every ring amount is a DAW parameter: automatable, and saved and restored with the DAW project.
- Moving LFOs never click or zip on any knob, including Output, Mix, Clean Low and the filter cutoffs.
- Filters and bells stay stable when moved by LFOs at full speed and full amount.
- Projects saved with version 2 open sounding the same: each bell's Move becomes the ring amount on its Freq, and its Shape, Speed and Start move into the matching LFO (shared Sync copied to all three).
- With Sync on, LFO positions follow the DAW song position, so the same bar sounds the same on every playback and bounce (Random wander and Jitter included).
- With Sync off, LFOs run freely from when playback starts.
- LFO shape changes are click-free: the Ramp's snap back and any jumps are smoothed.
- Switching Twist on or off never clicks.
- Twist volume jumps are handled by the existing Auto Gain and safety limiter (no extra Twist level control).
- Bells stay stable at every setting, including all three at +24 dB with Push on.
- Switching Character or Push crossfades so it never clicks.
- No Character ever collapses to silence: a quiet signal at full Drive and Push stays clearly audible.
- Projects saved with the old Character list open on the nearest new Character: Tape → Tape, Tube → Tube, Console → Edge, Synth → Heavy.
- Character acceptance (sine test): with a ~44 Hz sine at about −22 dBFS peak, each Character's Drive sweep (Push off and on) follows the shape and trend of its Decapitator reference curve in `decapitator-analysis.md`, not exact values.
- Sound-quality acceptance: in blind, volume-matched listening tests on drums, bass, vocals and synth, each Character is judged musically usable, with no audible aliasing at any Drive setting.

## Interface Layout
- **Twist strip across the full width of the top of the window:**
  - Twist display fills the strip from left to right.
  - Display shows the three bells moving live in their colours, the combined EQ curve, and a faint bar under each bell showing how far it can roam.
  - Each bell has a coloured dot: drag left/right = Freq, drag up/down = Gain, scroll wheel = Q. Dots and knobs always stay in sync.
  - Twist toggle at the top of the strip.
- **Middle row, below the Twist strip:**
  - Left: Bells table, three rows (one per bell, coloured name) with Freq, Gain and Q knobs. Column names appear once, above the rows.
  - Right: LFO section, three LFO panels side by side, all visible at once (no tabs).
  - Each LFO panel: coloured drag handle with its name, small picture of the current wave (including Curve), Shape dropdown, Sync toggle, Speed, Curve and Start knobs, Smooth, Jitter and Offset knobs, Unipolar toggle, and a label listing what it is connected to.
- **Assigning LFOs (simplified Serum):**
  - Drag an LFO's handle onto a knob to connect it; a ring in that LFO's colour appears around the knob.
  - Drag the ring to set the amount (either direction from the knob's position). Hovering or dragging a ring shows its amount.
  - A knob with several LFOs shows one ring per LFO.
  - Right-click a ring → Remove to disconnect.
- Below the middle row, three compact panels, left to right, following signal flow: **Drive | Filters | Output**, each with its knobs in a single row where possible.
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
- Square LFO shape
- Burst effects *(next version)*: "it can apply drastic and cool effects for short intervals (for example 1/8 note to 1/1 note)… Tape stops and slow downs and bit crush+gate fx. We can use an lfo to control the interval it turns on and off - like a one shot lfo." Effects: Tape stop, Slow down, Bitcrush + Gate, Stutter/Repeat, Reverse, Tape start. Started by both a repeating pattern and a one-shot button.
- LFO extras: Steps, Hold, Retrigger, Rate multiplier, speeds above 10 Hz *(shelved for a later version)*
- An LFO moving another LFO's settings
- A switch to put Twist before or after Drive
- An extra drive-and-squash stage after Twist *(owner chose more LFO options instead)*
- Surprise me / randomise
- Filter-curve or other visual displays, except the Twist display
- Custom artwork or branded hardware look
- Preset browser or built-in preset library
- Step sequencers or any automatic modulation besides Filter Follow and the three LFOs
- Sidechain input
- A/B comparison and in-plugin undo/redo
- MIDI learn
- Surround or multichannel layouts
- User-adjustable oversampling or reaction speeds
