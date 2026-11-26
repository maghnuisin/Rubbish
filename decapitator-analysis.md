# Decapitator style analysis (reference for new Rubbish modes)

Measured 2026-11-25 from screen recordings of Soundtoys Decapitator in Logic.
Source: pure sine, ~43.6 Hz (F1), −24.7 dB RMS, 0% THD. Tone, filters, Thump, Steep at default; Mix 100% wet; Output Auto on.
Drive positions read by eye from video frames (±0.5). THD = harmonics H2–H15 relative to the fundamental. Each figure is measured over a 1-second window.

## THD (%) vs Drive

### Punish OFF
| Drive | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 |
|---|---|---|---|---|---|---|---|---|---|---|---|
| A | 3 | 4 | 5 | 6 | 9 | 15 | 26 | 34 | 38 | 39 | 38 |
| E | 2 | 3 | 2 | 13 | 24 | 36 | 42 | 43 | 58 | 66 | 66 |
| N | 1 | 1 | 1 | 2 | 7 | 19 | 27 | 35 | 37 | 39 | 41 |
| T | 1 | 3 | 3 | 5 | 6 | 12 | 18 | 31 | 37 | 46 | 51 |
| P | 8 | 10 | 11 | 15 | 24 | 38 | 45 | 56 | 64 | 68 | 75 |

### Punish ON
| Drive | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 |
|---|---|---|---|---|---|---|---|---|---|---|---|
| A | 18 | 25 | 34 | 39 | 41 | 42 | 41 | 44 | 46 | 46 | 46 |
| E | 31 | 34 | 42 | 52 | 54 | 63 | 79 | 81 | 97 | 106 | ~90 |
| N | 17 | 25 | 29 | 35 | 40 | 45 | 44 | 45 | 47 | 52 | 55 |
| T | 30 | 35 | 43 | 50 | 59 | 59 | 65 | 60 | 57 | 52 | 43 |
| P | 26 | 35 | 44 | 50 | 57 | 64 | 72 | 72 | 70 | 55 | 46 |

(E was recorded with a faster sweep of about 1 s per step, so its values are less precise.)

## Summary per mode
| Mode | Onset | Harmonic balance as Drive rises | Top end at max Drive (energy >2 kHz, off / on) | Max-drive behaviour | Auto level change |
|---|---|---|---|---|---|
| A (Ampex tape) | Colours from 0 (pure H2) | Even → odd; crossover at Drive 4–5; H3 locks at about −10 dB | −44 / −29 dB | Hard ceiling about 40% (off) / 46% (on) | +6 dB mid, back down at top |
| E (EMI console) | Clean to 2, then steep | Even-dominant throughout (H2 up to −2 dB) | about −42 / −35 dB | Keeps rising, beyond 100% with Punish | −10 dB (overcompensates) |
| N (Neve) | Spotless to 3, sharp knee | Odd-dominant at all settings | −41 / **−22 dB** (brightest; H15 about −25 dB) | Keeps rising with Punish (about 55%) | −6 dB |
| T (Thermionic tube) | Gentle from 0 (H2 ≈ H3) | Balanced → slightly odd (Drive 2–4) → strongly even from 5 | **−65** / −40 dB (darkest) | Punish: peaks at Drive 6, then falls and becomes symmetric | −3 dB (most stable) |
| P (Pentode) | Colours at 0 (8%, H2 −22 dB) | Even → brief odd dip at Drive 3 → even; flips to odd at max Drive + Punish | −55 / −30 dB | Off: most distortion (75%). On: peaks at Drive 6–7, then falls | −5 dB |

## Notable behaviours to model
- **Harmonic nulls while sweeping:** individual harmonics briefly vanish (about −45 dB) at certain Drive positions (A: H2 at Drive 9 off and 6/9 on; T: H2 at about 3.5 off, H3 at about 8.5 off / 7.5 on; P: H3 dips near the top). This is why the character keeps changing as the knob turns.
- **Punish** works like starting several Drive steps higher, and mostly adds odd harmonics and upper-harmonic tail. It is a hardness/gain offset, not a different curve.
- **The tube styles (T, P) "fold over" when slammed with Punish:** THD peaks, then drops as the curve goes symmetric and odd-dominant.
- **Output Auto** does not hold level constant: the error ranges from about −3 dB (T) to −10 dB (E).

## Implications for Rubbish
- **A-style Tape:** an asymmetric H2 stage that dominates at low drive, followed by a soft clipper with a fixed ceiling.
- **E-style (Heavy):** a strongly asymmetric, H2-heavy curve with no ceiling. This is the most aggressive gain range and gives weight on bass.
- **N-style (Edge):** a hard-ish odd-symmetric clipper with a high threshold. It's clean until a sharp knee, with a long bright tail, so oversampling is most critical here.
- **T-style Tube:** asymmetry that increases with Drive (rising H2). At extreme drive it moves towards symmetric clipping.
- **P-style (Grit):** like T but with a bias that colours even at zero Drive, more distortion overall, a brighter top, and an odd flip at extremes.
- **Punish equivalent:** a pre-gain plus hardness offset. Rubbish's Push keeps its +20 dB and now also hardens the curve.

## Rubbish fit (2026-11-26)
Each Rubbish Character's curve was fitted to the tables above (Drive 0–10 mapped to Rubbish Drive 0–36 dB; Push = +20 dB plus a harder curve). For Tube, Heavy and Grit the off-centre bias grows with the gain, like a tube's bias shift, but is capped below the signal's own peak so a quiet signal can never be squashed to silence. Offline sine-test THD (%) of the built curves, for checking future changes:

| Drive | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 |
|---|---|---|---|---|---|---|---|---|---|---|---|
| Tape | 3 | 4 | 4 | 6 | 10 | 16 | 21 | 28 | 34 | 38 | 42 |
| Tape + Push | 19 | 26 | 32 | 37 | 41 | 43 | 44 | 45 | 45 | 45 | 45 |
| Tube | 1 | 2 | 3 | 4 | 7 | 12 | 20 | 28 | 38 | 46 | 50 |
| Tube + Push | 27 | 37 | 44 | 49 | 53 | 56 | 57 | 58 | 56 | 52 | 49 |
| Heavy | 2 | 3 | 3 | 12 | 20 | 29 | 40 | 50 | 59 | 66 | 71 |
| Heavy + Push | 35 | 31 | 41 | 63 | 69 | 74 | 77 | 79 | 80 | 81 | 81 |
| Edge | 1 | 1 | 1 | 2 | 6 | 17 | 27 | 34 | 39 | 42 | 44 |
| Edge + Push | 23 | 31 | 37 | 41 | 43 | 44 | 45 | 45 | 45 | 45 | 45 |
| Grit | 7 | 11 | 12 | 13 | 22 | 34 | 45 | 55 | 62 | 68 | 72 |
| Grit + Push | 31 | 39 | 46 | 52 | 57 | 60 | 62 | 63 | 63 | 62 | 62 |

Known gaps vs Decapitator, to be judged by ear: Tube and Grit only fold over gently at the top with Push; Heavy + Push tops out near 80% (Decapitator reaches about 100%); Edge + Push levels off near 45% (Decapitator keeps climbing to about 55%); no Character is as dark on top as Decapitator, which probably has built-in tone shaping.
