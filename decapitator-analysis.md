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

## P vs Grit at real Logic level (2026-11-26)
Measured in Logic ("Test rubbish", Bass track: Massive 44 Hz sine reaching the plugin at about −13 dBFS peak). Decapitator P, Punish off, Auto on, Tone centre, filters off, Mix 100%; bounced post-fader to `~/Desktop/P test drive 0…10.wav`. Grit rendered offline with the same sine 8.5 dB hotter (to match the plugin input), Drive = Decapitator step × 3.6 dB, Push off, Auto Gain on. Harmonics in dB relative to the fundamental; THD = H2–H15.

| Drive | P THD % | P H2 | P H3 | P H4 | P H5 | Grit THD % | Grit H2 | Grit H3 | Grit H4 | Grit H5 |
|---|---|---|---|---|---|---|---|---|---|---|
| 0 | 6 | −25 | −40 | −56 | −62 | 18 | −16 | −24 | −32 | −48 |
| 1 | 9 | −21 | −37 | −51 | −59 | 21 | −16 | −18 | −26 | −35 |
| 2 | 10 | −20 | −32 | −44 | −56 | 25 | −20 | −13 | −25 | −24 |
| 3 | 11 | −20 | −26 | −38 | −55 | 31 | −34 | −11 | −36 | −19 |
| 4 | 13 | −37 | −18 | −36 | −38 | 37 | −26 | −10 | −28 | −16 |
| 5 | 25 | −15 | −16 | −27 | −30 | 41 | −19 | −10 | −20 | −16 |
| 6 | 39 | −10 | −17 | −16 | −48 | 45 | −16 | −10 | −17 | −17 |
| 7 | 45 | −7 | −33 | −18 | −27 | 48 | −15 | −10 | −15 | −18 |
| 8 | 57 | −6 | −25 | −13 | −17 | 49 | −14 | −11 | −14 | −19 |
| 9 | 65 | −4 | −16 | −22 | −18 | 50 | −13 | −11 | −14 | −20 |
| 10 | 63 | −5 | −17 | −18 | −20 | 51 | −13 | −11 | −14 | −21 |

What it shows:
- **Drive 0–4:** Grit is 2–3× dirtier than P and dominated by odd harmonics (H3, H5); P stays gentle and mostly 2nd-harmonic. Grit is far too sensitive to input level.
- **Drive 5–6:** about the same amount, different mix.
- **Drive 7–10:** P keeps climbing to about 65% and becomes **strongly lopsided**: the 2nd harmonic is almost as loud as the note itself (−4 to −7 dB). Grit levels off at about 50%, stays fairly even-sided (H3 louder than H2) and stops changing. This is the "missing grit" at the top.
- The original fit (tables above) was done at about −22 dBFS and matched only total THD, so it missed both.

### Continuous Drive sweep (2026-11-26)
Owner asked for a sweep rather than steps, to catch the detail between whole numbers. Decapitator P (Punish off) Drive was automated 0 → 10 over 16 bars in Logic and bounced (`~/Desktop/P sweep drive 0 to 10.wav`); Grit was swept the same way offline (0 → 36 dB). Every 0.1 Drive step is in `measurements/p-sweep-punish-off.csv` and `measurements/grit-sweep-push-off.csv`. The sweep agrees with the stepped recordings at whole numbers (e.g. Drive 6: 39.4% both ways).

Fine detail in P that the steps half-hid:
- **0 → 3:** creeps up slowly (7% → 11%), always mostly 2nd harmonic (even harmonics 6–16 dB above odd).
- **3 → 4.5: a "turnover" zone.** The 2nd harmonic briefly collapses (to −37 dB at 4) and the 3rd takes over, so the tone flips to odd/hollow for a moment, and total distortion even dips (11% → 10% at 3.5) before it takes off.
- **4.5 → 7:** distortion shoots up (20% → 45%) and swings back to strongly even (2nd harmonic −15 → −7 dB).
- **7 → 10:** very lopsided and still climbing, peaking about 71% around 9.5 then easing to ~62% at 10; the 2nd harmonic sits only 4–6 dB under the note.

Grit by contrast rises smoothly and steadily from 18% to 49%, is odd-heavy (hollow/buzzy) from 1.5 to 7, never gets the big 2nd harmonic, and barely changes after 7.

### Continuous Drive sweep with Punish / Push on (2026-11-26)
Same method, Decapitator P with Punish on (`~/Desktop/P Punish sweep drive 0 to 10.wav`) vs Grit with Push on. Data: `measurements/p-sweep-punish-on.csv`, `measurements/grit-sweep-push-on.csv`.

| Drive | P+Punish THD % | P+Punish H2 / H3 dB | Grit+Push THD % | Grit+Push H2 / H3 dB |
|---|---|---|---|---|
| 0.5 | 24 | −18 / −14 | 43 | −25 / −9 |
| 1 | 29 | −13 / −15 | 44 | −22 / −10 |
| 2 | 44 | −9 / −17 | 45 | −19 / −10 |
| 3 | 54 | −6 / −33 | 46 | −17 / −10 |
| 4 | 60 | −5 / −20 | 47 | −16 / −10 |
| 5 | 63 | −5 / −16 | 48 | −15 / −10 |
| 6 | 71 | −5 / −16 | 48 | −15 / −10 |
| 7 | 73 | −4 / −15 | 48 | −15 / −10 |
| 8 | 68 | −5 / −17 | 48 | −15 / −10 |
| 9 | 59 | −7 / −28 | 48 | −15 / −10 |
| 10 | 48 | −12 / −12 | 48 | −15 / −10 |

What it shows:
- **P + Punish** climbs from about 24% to a peak of about 73% at Drive 7, then **folds back** to about 48% at 10, where the huge 2nd harmonic shrinks and the 3rd/4th catch up (it changes colour rather than just getting louder). It is strongly lopsided (2nd harmonic 4–7 dB under the note) from Drive 3 to 8.
- **Grit + Push barely responds to Drive at all:** 41–48% across the whole sweep, odd-heavy at the bottom and then frozen from about Drive 4. Push already slams it into its ceiling, so the Drive knob does almost nothing with Push on. No fold-back, no lopsided 2nd harmonic.

## Grit refit (2026-11-26)
Grit's eight voicing numbers were refitted by computer search against the P sweeps above (Punish off and on, every 0.25 Drive, matching total distortion and the 2nd–5th harmonics), at the real Logic level (about −13 dBFS peak). The structure of the curve is unchanged. Old: `{14.092, 0.133, −0.405, −0.152, 2.484, 0.000, 0.701, 0.900}`; new: `{2.198, 0.319, −0.824, −0.417, 2.166, −1.021, 0.695, 0.950}` (sensitivity dB, bias at 0, bias across Drive, bias from Push, hardness, hardness from Push, bias follows gain, bias limit). The Grit rows in the "Rubbish fit" table above are out of date.

Checked in the full plugin (same sine, same level):

| Drive | P | new Grit | P + Punish | new Grit + Push |
|---|---|---|---|---|
| 1 | 9% | 9% | 29% | 34% |
| 2 | 10% | 11% | 44% | 44% |
| 3 | 11% | 11% | 54% | 52% |
| 4 | 13% (hollow dip) | 14% (hollow dip) | 60% | 60% |
| 5 | 25% | 25% | 63% | 65% |
| 6 | 39% | 38% | 71% | 69% |
| 7 | 45% | 50% | 73% | 70% |
| 8 | 57% | 60% | 68% | 71% |
| 9 | 65% | 68% | 59% | 70% |
| 10 | 62% | 73% | 48% | 69% |

The 2nd/3rd harmonic balance follows P too (including the hollow turnover around Drive 3.5–4 and the very lopsided top). Remaining gap: with Push on, P folds back at Drive 9–10 (to about 50%) and Grit stays at about 70%. A version that folded back made low-Drive Push too dirty (40% vs 24%), so it was not used. Never silent from −40 to 0 dBFS input at any Drive/Push.

Note against prd.md: Push on Grit now makes it *more lopsided (even harmonics)* and a softer knee, matching P + Punish, rather than "more odd harmonics"; and the fold-over at the top is weak. Owner to decide whether prd.md's Push wording should change.

## Why is P still "grittier"? Full-spectrum check (2026-11-26, after the Grit refit)
Owner, after listening to the refitted Grit: "Still not as gritty, dirty. There is more harmonics in the P version that we are not matching."
Compared every harmonic up to 20 kHz and the energy between harmonics, on the steady recordings (`P test drive N.wav`) and the two automation sweeps, against the new Grit rendered at the same level.

- **Upper harmonics, Punish off:** around Drive 4.5–7.5, P has a long bright tail: harmonics 17–40 (about 750 Hz–1.75 kHz) and above are **15–20 dB louder** than Grit's. At Drive 8–10 the tails are about equal (Grit slightly stronger). With Punish/Push on, Grit already has as much or more upper-harmonic energy than P.
- **Noise between harmonics:** in the sweeps P's grows to about −42 dB (vs Grit's −90 dB), but in the steady recordings it is only about −105 dB (same as Grit). So it only happens **while Drive is moving** (Decapitator reacting to automation steps), not while the knob is still.
- **Loudness:** P with Auto is quieter than Grit at high Drive (−30 vs −25 dB RMS at Drive 10), so loudness doesn't explain it.
- **Wave shape over one cycle:** both are a sharp jump followed by a slow slope; P is lopsided the opposite way (its sharp peak is on the negative side). On a single sine this sounds the same, but it can matter once a sound has several partials or is mixed with the clean signal.
- **Not testable with one low sine:** whether P treats higher notes/partials differently (built-in tone shaping before or after the clipping), and how it reacts to real playing (attacks, notes changing). A single 44 Hz sine has matched P about as far as it can.

## P on higher notes and a real bass, and the second Grit refit (2026-11-26, later)
Recorded in Logic (Desktop): P, Punish off, Auto on, Drive automated 0→10 over bars 1–17, on the Massive sine transposed to about 175, 700 and 2800 Hz (`P sweep 175Hz.wav`, `P sweep 700Hz.wav`, `P sweep 2800Hz.wav`), and on the Massive preset "Analog Classique 1" holding F2 (87 Hz) (`P sweep bass F2.wav`, plus the dry preset as `Clean bass F2.wav`).

- **P does no tone shaping before or after its clipper.** Total distortion and the 2nd–9th harmonics follow the same path against Drive at 44, 175, 700 and 2800 Hz (within a few dB). No aliasing either.
- **On the real bass** (octave-band energy, averaged per Drive step) the gap was the same as on the sine: between Drive 3 and 8 the old Grit had more low-mid harmonics, while P had 10–20 dB more energy above about 2.4 kHz.
- **Why:** P's upper-harmonic tail falls off at a steady rate as harmonics get higher, as a curve with a definite "bend point" does. Grit's curve is perfectly smooth, so its tail stays tiny until it is driven hard and then appears all at once.
- **Second refit (numbers only, same curve design):** the search now also targets the upper tail (harmonics 10–19 and 20–40, data in `measurements/p-tail-*.csv` and `grit-tail-*.csv`). New Grit: `{0.000, −0.841, 2.275, 0.832, 1.604, 0.280, 0.497, 0.609}`. Softer knee, and the bias flips from one side to the other around Drive 13 dB (Decapitator ≈ 3.7), which gives P's hollow turnover.
  - Push off, Drive 4.5–10: tail now within about 3 dB of P (was 15–20 dB short at 4.5–6.5). Total distortion and 2nd harmonic still follow P; the 3rd harmonic is weaker than P's at Drive 5–6.5.
  - Push on: tail matches P; distortion now eases from about 71% (Drive 6) to 62% (Drive 10), part of P + Punish's fold-back (P goes to about 48%).
  - Real bass: the top end at Drive 4–10 is now at or slightly above P's.
- **Side effect at low Drive (0–2.5, Push off):** a faint upper buzz (harmonics 10–40 at about −60 to −70 dB vs P's −80 to −90). Cause: the bias limit follows the signal's peak with a 0.1 s release; between the peaks of a low note it sags a little, so the curve wobbles once per cycle. The new numbers lean on that limit at low Drive. Modelling it in the search didn't find numbers that avoid it. A follower that holds its level longer (about 2 s in the model) removes most of it, but that is a change to how the plugin works (and also affects Tube and Heavy), so it is waiting for the owner.
- Checked: never silent from −40 to 0 dBFS input at any Drive/Push; auval passes.

## "High fizz and a phaser-like wobble" (2026-11-27)
Owner: P has a high fizz and something like pitch modulation or a phaser that comes forward in the last quarter of the Drive range. Measured on the Logic recordings (non-harmonic content, i.e. anything that isn't a multiple of the note):
- **Aliasing at high Drive (steady knob).** 2800 Hz note at Drive 10: tones at 22,855 / 20,062 / 17,268 Hz (−36 / −43 / −63 dB vs the note), exactly harmonics 9, 10 and 11 folded back around 24 kHz. Also at 700 Hz (−73 dB). None on the 44 Hz sine (its harmonics die out first). So P under-oversamples its clipper. Rubbish oversamples to avoid exactly this (prd.md requires no audible aliasing).
- **Wobble while Drive moves.** During the automation sweeps, every harmonic gets sidebands about 7–10 Hz either side (−38 to −47 dB vs the note at Drive 5–7.5), which disappear when Drive is still. That is an audible flutter tied to turning the knob. Rubbish's smooth Drive glide doesn't do this.
- On the dark F2 bass preset neither shows clearly (little high content to fold back). Which one the owner hears is not yet known.

## Harmonics vs Drive map: the sweeping notch (2026-11-27)
Every harmonic up to the 40th was measured every 0.1 Drive from the P sweep recordings (44 Hz and 175 Hz notes, Punish off) and from Grit rendered offline with the same ramp and level. Data: `measurements/p-harmonic-map-*.csv`, `measurements/grit-harmonic-map-*.csv`. Pictures: `measurements/harmonic-map-*-P-left-Grit-right.png` (Drive 0→10 left to right, harmonic 1 at the bottom to 40 at the top, dark = quiet).

- **P has gaps that climb with Drive.** Single harmonics drop 8–30 dB below their neighbours, and the gap moves steadily up the harmonic series as Drive rises. On the 175 Hz note, one track runs from about the 17th harmonic at Drive 4.7 to the 23rd–29th at Drive 5.9 (about 3–5 kHz), followed by more tracks at 6.5–8 and 8.9–10. The 44 Hz note shows the same tracks at the same Drive values. With the knob still, the gap sits on one harmonic; while Drive moves, it slides upward, which fits the owner's "narrow accentuated notch sweeping up the spectrum". This is a property of P's curve, not of automation.
- **Grit's gaps freeze.** From about Drive 6 up, Grit's gaps stay on the same harmonics (10, 13, 23, 26, 36 at 44 Hz; 7, 10, 17, 27, 34, 37 at 175 Hz) whatever the Drive, so its tone stops changing. The cause is that Grit's bias is capped at a fixed fraction of the signal's peak, and from about Drive 6 it sits on that cap. The bias then grows exactly in step with the signal, so the waveform's shape, and its harmonic pattern, stops evolving. P's lopsidedness keeps changing relative to the signal all the way up.
- Between Drive 2.7 and 4.7 Grit has a busy chequerboard of gaps (its bias changing sign), not P's smooth climbing tracks.
- So the "movement" P has while turning Drive needs Grit's lopsidedness (and possibly knee) to keep changing relative to the signal at high Drive, not just the right harmonic levels. That touches the bias cap, which also keeps quiet notes from going silent, so it needs care and the owner's OK.
- Side note: P's map also shows thin vertical stripes at a few Drive values (around 1.1, 2.3, 3.7), short bursts of energy across all harmonics. They fit stepped Drive updates during automation (the 7–10 Hz flutter), a separate effect.

### Trying to make Grit's gaps move like P's (2026-11-27, offline only, no code changed)
Owner OK'd retuning how Grit's lopsidedness follows Drive. Fitted by computer search against the P maps (44 / 175 / 700 Hz Punish off, 44 Hz Punish on), using an exact offline copy of Grit's maths (checked: within 0.09 dB of the full render) plus two candidate new numbers: the bias limit changing across Drive, and the knee hardness changing across Drive.
- The best candidate `{−0.971, −0.802, 1.832, 1.531, 1.726, 0.279, 0.479, limit 0.210 → 0.912 across Drive, hardness −0.402 across Drive}` matches P's 2nd–5th harmonics better (average error 5.3 → 4.3 dB, including P's hollow dip near Drive 4 and its 3rd-harmonic dip near 7) and the tail slightly better (6.9 → 6.2 dB). Its gaps move between Drive 3.5 and 7.
- **But from about Drive 8 up, Grit's gaps still freeze**, which is the range where the owner hears P's effect most. A knee that sharpens with Drive didn't fix it either, and it worsened the harmonic match.
- Why (reasoning): at high Drive Grit's wave becomes a lopsided, rounded "square", and its shape stops changing. P's climbing gaps at the top look like the effect of sharp corners on the wave's edges, which keep getting steeper as Drive rises. Grit's soft-clip curve with a capped bias can't produce that with any numbers tried. Getting it would need a different curve design for Grit (e.g. a harder-cornered clipper core), which is a bigger change than retuning.
- Nothing was put into the plugin.

### New Grit design tried offline: a hard ceiling with corners (2026-11-27)
Added a hard ceiling after Grit's soft curve (different heights for the two halves allowed, moving with Drive and Push), so the wave gets sharp corners where it hits it. With the ceiling at 1 it is exactly today's Grit. Fitted the same way against P's maps.
- The overall match is no better than the retune without corners. Best ceiling version `{−1.036, −0.807, 1.831, 1.553, 1.951, 0.041, 0.477, 0.251, 0.698, −0.613, ceiling 0.635 +0.319 across Drive, negative side ×1.46, Push +0.330}`.
- Grit's gaps above Drive 8 still lock onto fixed patterns. Looking closer, P's top-range gaps are mostly evenly spaced groups (e.g. harmonics 13, 16, 19…) that shift by a harmonic or so as Drive moves. They come from how lopsided the wave is, and recording noise from P's stepped automation blurs that range. The numbers can't cleanly say which version is closest.
- Listening files made instead (same Massive bass F2 as `P sweep bass F2.wav`, Drive 0→10 over 42.7 s, 4x oversampled offline copy of Grit's maths, loudness matched to P window by window): `~/Desktop/rubbish Test files/Grit candidates (bass F2, Drive 0 to 10)/` → 1 Decapitator P, 2 Grit now, 3 Grit retuned, 4 Grit with corners.
- Not yet checked for the corner version: aliasing on high notes, never-silent behaviour at all levels.
- **Owner's pick by ear: "3 Grit retuned".** Built into the plugin 2026-11-27. Grit now `{−0.971, −0.802, 1.832, 1.531, 1.726, 0.279, 0.479, 0.210}` plus two new Voicing numbers (0 for every other Character, so they are unchanged): bias limit +0.702 across Drive (0.21 → 0.91 of the signal peak) and hardness −0.402 across Drive. Checked offline: the fundamental never drops more than 4 dB below the input from −40 to 0 dBFS at any Drive/Push (previous Grit: 8 dB); aliasing on a 2.8 kHz note at full Drive is unchanged (about −57 dB vs the note). auval passes.
