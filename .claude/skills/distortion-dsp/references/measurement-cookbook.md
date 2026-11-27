# Measurement and diagnosis cookbook

How to compare a reference saturator (e.g. Decapitator P in Logic) with Rubbish, and how to avoid fooling yourself. Each recipe says what it reveals. Combine several; no single test captures "character".

## 0. Set-up rules (most past mistakes came from here)

- **Know the real input level.** Logic's track fader, the main fader and the bounce point all change it. On 2026-11-26 the sine reached the plugins at about −13 dBFS peak while the recordings looked 8 dB quieter, because they were post-fader. State the level as dBFS peak *at the plugin input*. Render Rubbish offline at that same level.
- **Auto gain off, or measured.** Decapitator Auto and Rubbish Auto Gain change levels with Drive, so level-match the outputs on a reference sine before comparing spectra or listening.
- **Latency-align before nulls.** Measure each plugin's delay with an impulse, then align to the sample.
- **Same sample rate.** Aliasing depends on it, and Logic sessions here are 48 kHz.
- **Coherent analysis.** Use a whole number of cycles in the FFT with a rectangular window, or a Blackman-Harris/flat-top window otherwise, so leakage doesn't hide high harmonics. [Uncited background]
- **Reference recordings move.** Massive presets with detune or movement need band averages, not single windows.
- **pluginval / auval check stability and format conformance only**, not sound.

## 1. Steady sines across level (the fingerprint)

- **What:** 100 Hz, 1 kHz and 5 kHz sines, input from −40 to 0 dBFS in 3–6 dB steps, at several Drive settings.
- **Plot:** H2–H9 and THD vs level, plus tail bands (H10–19, H20–40).
- **Reveals:** the curve's level-dependence, even/odd balance, the onset knee and harmonic nulls.
- **Note:** REW says stepped sines are more accurate than sweeps for low distortion and high harmonics [Documented: REW help].

## 2. Fine Drive map (for "lumpy" character and notches)

- **What:** at a fixed level, render or record steady sines at Drive steps of 0.1–0.25 (no automation). Plot H1–H40 as a heat map (Drive × harmonic).
- **Reveals:** the harmonic nulls that walk up the series as Drive rises (`harmonics-and-curves.md` §3) and turnover zones (P's hollow dip at Drive 3–4.5).
- **Compare** the maps for P and Rubbish directly. Matching the *path of the nulls* matches the "movement" heard while turning the knob.

## 3. Synchronized swept sine (Farina / Novak)

- **What:** an exponential sweep. Deconvolution separates each harmonic order's response along the time axis.
- **Sources [Documented]:** Farina, AES 2000; Novak et al., JAES 2015 (synchronisation step needed for correct higher-harmonic phase, Matlab code included); Novak et al. 2010 applied it to overdrive pedals.
- **Reveals:** H1–H5 vs frequency, i.e. hidden pre/post tone shaping or a frequency-dependent "Response". Run it at 2–3 levels.

## 4. Intermodulation

- **Standards [Documented: RaneNote 145; Audio Precision "More about IMD"]:**
  - **SMPTE:** 60 Hz + 7 kHz at 4:1; state the peak level.
  - **DIN:** 250 Hz + 8 kHz.
  - **CCIF:** two close high tones, measuring the difference product.
- **What each shows:** SMPTE/DIN show how bass modulates highs (crunch on real bass); CCIF shows high-frequency products and doubles as an aliasing probe.
- **Also try** a musical two-tone (root + fifth, or a bass note + a high partial).

## 5. Aliasing

- **What:** a steady 5–15 kHz sine at high Drive (+Push), and a slow sweep to Nyquist.
- **Look for:** non-harmonic lines that move **down** as the input rises.
- **Confirm:** change the session rate (44.1 → 96 kHz). Aliases move or vanish; real harmonics stay.
- **Metric [Documented: Esqueda et al. 2016]:** inharmonic-to-harmonic energy ratio (an SNR). Plugin Doctor's single-frequency mode shows non-integer components directly [Documented, manual via search].

## 6. Bursts (memory, bias shift, attack vs sustain)

- **What:** quiet sine → burst 20–30 dB louder for 50 / 200 / 500 ms → quiet sine again.
- **Compare:** harmonics and DC before, during and after the burst.
- **Reveals:** dynamic bias, sag, blocking and the recovery time constant. These are invisible to THD sweeps [Documented: Aiken on blocking; recipe adapted, Inference].
- **Rubbish note:** Rubbish's bias-limit peak follower (0.1 s release) is itself a memory; this test shows it.

## 7. Null / delta test on music

- **What:** latency-align, fit gain (least squares), subtract.
- **Report:** the residual level and listen to it.
- **Use for:** confirming two renders are identical, and isolating what changes while a parameter moves (automated render minus static render at the endpoint).

## 8. Parameter-motion tests

See `parameter-changes.md` §6: sideband spacing vs block size and sample rate, ramp speed, hand vs drawn automation, Auto on/off, Mix 100/50%, pink noise with automated Drive.

## 9. Perception checks

- **[Documented: Geddes & Lee, AES 2003]** Listener ratings didn't track THD or IMD but did track their GedLee metric (r = 0.95), which weights higher orders more. [The weighting detail is from general knowledge of the paper, not re-read.]
- **[Documented: Tan, Moore & Zacharov, JAES 2003]** A multitone-based measure predicts perceived distortion reasonably well.
- **Practical:** weight high harmonics and IMD more when judging harshness or grit. Always finish with loudness-matched listening, then blind A/B on drums, bass, vocals and synth (prd.md acceptance).

## 10. Tools

- **Plugin Doctor (DDMF):** THD, IMD, sweeps and aliasing views; loads plugins directly.
- **REW:** stepped sine, multitone (rectangular window, same clock).
- **Python with numpy/scipy:** not installed on this Mac (checked 2026-11-27). Previous sessions wrote small C++ analysers instead (peak/harmonic tools in a session scratchpad). A short C++ single-cycle DFT is enough for harmonic tables.
- **Logic offline bounce** for references, noting fader positions (post-fader bounces include the faders).
