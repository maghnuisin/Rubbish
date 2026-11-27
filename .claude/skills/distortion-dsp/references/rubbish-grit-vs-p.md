# Applying this to the open question: Grit vs Decapitator P

Status as of 2026-11-27: analysis only. No code changed. Re-check `build-log.md` and testing note 8 (2026-11-26 session) for newer results before acting.

## The observations

1. With P's Drive **held still** at 8–9 there is a **high fizz**. Recordings show folded harmonics (2800 Hz note, Drive 10: 22,855 / 20,062 / 17,268 Hz at −36 / −43 / −63 dB), i.e. aliasing. Rubbish oversamples, so it has none.
2. **While P's Drive is moving** (by hand or automation), the owner hears "a narrow accentuated notch sweeping up the frequency spectrum". It stops when the knob stops.
3. In the automation sweeps, every harmonic has **sidebands about 7–10 Hz either side** (−38 to −47 dB re note at Drive 5–7.5), which vanish when Drive is still. Between-harmonic energy reaches about −42 dB in the sweeps vs about −105 dB in the steady recordings.
4. P otherwise treats 44/175/700/2800 Hz notes the same (no fixed tone shaping). Its tail falls like a curve with a definite bend. It has a "turnover" around Drive 3–4.5 where H2 collapses (−37 dB at Drive 4), and it becomes strongly lopsided at the top.

## Likely causes, ranked (all hypotheses)

### H1. The curve's own harmonic nulls, sweeping as Drive changes — **CONFIRMED 2026-11-27**

> Result: the map from the existing P sweeps shows null tracks climbing with Drive (175 Hz note: about the 17th harmonic at Drive 4.7 to the 23rd–29th at 5.9, i.e. 3–5 kHz; more tracks at 6.5–8 and 8.9–10; the same on 44 Hz). Grit's nulls freeze from about Drive 6 because its bias sits on the peak-follower cap, so bias/signal is constant and the waveform shape stops evolving. See `decapitator-analysis.md` ("Harmonics vs Drive map") and `measurements/harmonic-map-*`. The analysis tool (`hmap.cpp`, `nullmap.cpp`) lived in a session scratchpad: a 0.25 s Blackman-Harris DFT at exact harmonic frequencies, every 0.1 Drive, plus an offline copy of the Grit curve maths. Rebuild it if needed.
- **Mechanism (`harmonics-and-curves.md` §3):** curves with a sharp bend, or any bias, have harmonics that drop into deep nulls at particular drive levels. As drive rises, the nulls climb the harmonic series (hard clip: first null at roughly harmonic 3 × drive; biased tanh: 11 → 13 → 17 → 21 → 29 → 37 for drive 1.5 → 8).
- **Why it would be heard only while moving:** a still knob parks the null on one harmonic, which is hard to notice. A moving knob slides it across neighbouring harmonics, which is heard as a narrow notch sweeping **up** when Drive rises. That is exactly the description.
- **Supporting evidence:** P has a definite bend and a strong, drive-dependent bias, so it should have many null tracks. Its documented H2 collapse at Drive 4 is one such null.
- **Why Grit might not show it audibly:**
  - In theory Grit is biased, so it has nulls too [Computed for a biased h=1.6 curve].
  - But Grit's tail is quieter at the drives where the owner listens.
  - Its bias path across Drive differs (it flips sign around 13 dB).
  - Its peak-follower bias cap reshapes the curve continuously.
  - So the null tracks may be shallower, sit elsewhere, or lie in harmonics too quiet to hear. **Untested.**
- **Test (no new recordings needed):**
  1. From `~/Desktop/rubbish Test files/P sweep drive 0 to 10.wav` (and the 175/700/2800 Hz sweeps), build a heat map of H1–H40 against Drive. Use short windows of a few periods; automation is slow enough.
  2. Do the same for Grit rendered offline with the same ramp and input level (about −13 dBFS peak).
  3. Diagonal "valleys" moving to higher harmonics as Drive rises confirm H1. Comparing the two maps shows whether Grit has the same tracks.
  4. Confirm without any automation by rendering or recording steady sines at fixed Drive steps of 0.1–0.25 (`measurement-cookbook.md` §2). The nulls must still be there.
  5. Pink noise through P while automating Drive should show **no** sweeping notch if H1 is right (no harmonics, no harmonic nulls).
- **If confirmed:** matching it is ordinary curve tuning, within prd.md (Grit "changes character as Drive is turned"). Fit the curve to the null tracks (knee sharpness h and bias path vs drive) as well as to THD and tail. A sharper knee will raise aliasing risk, so re-check with a high sine.

### H2. Stepped Drive updates inside P (zipper / AM) — best fit for the *7–10 Hz sidebands*
- **Mechanism (`parameter-changes.md` §1):** if P updates its drive in steps at about 7–10 Hz, every harmonic gets sidebands with the same Hz spacing, and beating sidebands sound like a flutter or "phasey" roughness. It explains observation 3, but on its own not a narrow sweeping notch.
- **Test:**
  1. Automate Drive with a linear ramp on a 220 Hz sine.
  2. Change the I/O buffer (128 vs 1024), Process Buffer Range, Sample Accurate Automation and session rate (44.1/48 kHz).
  3. Change the ramp length (1 / 4 / 16 s), and compare hand-turning with drawn automation.
  4. Spacing that follows fs / block → block-rate stepping; spacing that never changes → an internal timer; sideband *level* that rises with ramp speed → stepping.
- **If confirmed:** this is a by-product of how P handles parameter changes. Copying it means deliberately adding zipper modulation, which conflicts with prd.md ("All parameters are smoothed so moving them never clicks or zips"). That would be the owner's decision and a prd change.

### H3. Auto output reacting with lag or steps
- **Mechanism:** a gain trailing Drive gives AM sidebands.
- **Test:** repeat the automation with Auto off (fixed Output). Sidebands dropping sharply → H3.
- **Note:** Rubbish's Auto Gain should use the same smoothed Drive so it can't do this.

### H4. A tone filter inside P whose setting follows Drive
- **Mechanism:** would make a moving peak or notch.
- **Evidence against:** the same harmonic path at four note pitches argues against fixed shaping, but a *drive-dependent* filter isn't fully ruled out.
- **Test:** pink noise with automated Drive. A moving notch in noise → H4 (or a parallel-path comb).

### H5. Parallel-path comb (Mix < 100%, internal parallel stages)
- Unlikely: tests used Mix 100%.
- **Test:** Mix 50% vs 100% with noise.

### H6. Moving nulls among the aliases
- On high notes, a null climbing above Nyquist folds back and moves **down** among the aliased lines. It could add to the "phaser" impression on bright sounds. The owner hears the notch moving **up**, so this is secondary.

### Follow-up (2026-11-27)
- Retuning Grit so its bias limit grows with Drive (0.21 → 0.91 of the peak) and its knee softens slightly made its gaps move between Drive 3.5 and 7. It also matched P's H2–H5 better.
- Above about Drive 8 the gaps still mostly lock, because the bias-to-signal ratio settles.
- A hard-ceiling (corners) design was no better.
- The owner chose the retune by ear (offline renders in `~/Desktop/rubbish Test files/Grit candidates…`), and it is now in the plugin.
- Lesson: on paper, P's top-range gaps are evenly spaced groups set by how lopsided the wave is relative to the signal, blurred by P's stepped automation. Listening tests settled it faster than fitting.

## The fizz (Drive still)

- This is aliasing (H-alias). Matching it conflicts with prd.md ("no audible aliasing at any Drive setting").
- Options for the owner (decision needed, nothing built):
  - (a) Accept the difference.
  - (b) Get more legitimate high-harmonic energy from a sharper knee, which keeps everything harmonic.
  - (c) Change prd.md to allow a controlled-aliasing option (prd currently also excludes user-adjustable oversampling).
- Explain to the owner that aliasing produces out-of-tune high tones that move against the notes.

## Suggested order

1. H1 heat maps from the existing sweep files (cheap, offline, no Logic).
2. A short Logic session for H2/H3/H4: buffer and rate changes, Auto off, pink noise. Restore the project afterwards and don't save.
3. Report to the owner in plain words: what the notch is, whether Grit can do it by curve tuning alone, and the prd.md decisions for zipper-style flutter and aliasing.
