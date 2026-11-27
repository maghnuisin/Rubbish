---
name: distortion-dsp
description: >-
  Expert, source-cited knowledge of distortion and saturation DSP for the Rubbish plugin: how a waveshaper curve's shape, knee and bias set the harmonic spectrum; stateful/dynamic distortion (bias shift, sag, blocking, tape hysteresis); circuit modelling (DK method, WDF, state-space, neural); aliasing and antialiasing (juce::dsp::Oversampling, ADAA, polyBLAMP); parameter smoothing and what happens to the sound while Drive moves (zipper sidebands, moving harmonic notches, comb filtering); DC blocking; auto gain; how commercial saturators (Decapitator, Saturn, Klanghelm, KClip, Inflator, Airwindows) are documented; and a measurement/diagnosis cookbook. Use this whenever work touches a Rubbish Character or any distortion curve, Drive/Push behaviour, matching or comparing against a reference saturator such as Decapitator, "it's not gritty enough", fizz, harshness, aliasing, oversampling, drive or parameter smoothing, zipper noise, sidebands, a notch or phaser effect while turning a knob, DC offset, Auto Gain, or planning measurements of distortion, even if the user doesn't say "DSP".
---

# Distortion / saturation DSP

Knowledge base for designing, matching and debugging distortion in Rubbish (JUCE 9, C++17, `Source/dsp/`). It summarises published research and maker documentation, gathered in November 2026, with links. Use it to reason about *why* a distortion sounds the way it does and *how to test* a hypothesis before changing code.

Ground rules that come from the project and still apply here:
- `prd.md` decides what gets built. This skill explains options; it never authorises a feature. In particular, prd.md requires **no audible aliasing**, so "add aliasing for grit" needs the owner's decision.
- The owner has no DSP background. Translate findings into plain language ("the curve has a sharp bend, which makes bright high harmonics") and say how to hear the result.
- Labels used throughout: **[Documented]** = a cited source says it; **[Computed]** = checked numerically in this project's research (reproducible, not published); **[Inference]** = reasoning; **[Uncertain]** = unverified. Keep these labels when you pass knowledge on.

## Which reference to read

| You are working on… | Read |
|---|---|
| Curve shape, knee, bias, even/odd balance, harmonic nulls, "brightness" of the tail, stateful bias/sag/tape, pre/post EQ, DC, auto gain | `references/harmonics-and-curves.md` |
| Fizz, aliasing, oversampling factor or filter type, latency, ADAA, polyBLAMP | `references/aliasing.md` |
| Smoothing Drive/Push/filters, zipper noise, sidebands, a notch or phaser heard only while a knob moves, crossfades, filter stability under modulation | `references/parameter-changes.md` |
| Choosing a modelling approach (DK, WDF, state-space, port-Hamiltonian, neural) and how each handles moving knobs | `references/circuit-modeling.md` |
| What Decapitator / Saturn / Klanghelm / KClip / Inflator / Ableton / Airwindows publicly document | `references/commercial-saturators.md` |
| Planning a measurement or comparing Rubbish to a reference recording | `references/measurement-cookbook.md` |
| The open Grit-vs-P question (sweeping notch while Drive moves; fizz at high Drive) | `references/rubbish-grit-vs-p.md` |
| Full citation list with one-line summaries | `references/sources.md` |

## Core ideas (the 12 things to keep in mind)

1. **Symmetry sets the harmonic family.** An odd-symmetric curve gives only odd harmonics; any asymmetry (input bias, different positive/negative curves) adds even harmonics *and* DC. With a fixed bias, H2 leads at low drive, H3 overtakes around moderate drive, and H2 sinks again at heavy drive, because the signal swamps the bias. To keep even harmonics at high drive the bias must grow with level. [Documented: Pakarinen & Yeh 2009; Computed]
2. **Knee smoothness sets the upper-harmonic tail.** If the curve is smooth up to its k-th derivative and the next one jumps at the knee, harmonics fall about 6·(k+2) dB/octave: hard clip −12, cubic soft clip −18, quintic −24. Analytic curves (tanh, atan, x/√(1+x²)) fall off faster with every octave. Harmonics 3–9 look nearly alike across curves at equal drive; the curves differ mainly above about the 15th–20th harmonic, where "fizz" and "bite" live. [Documented: Fourier theory; Computed]
3. **A matched sine test is not a matched sound.** Two curves can agree on THD and H2–H5 and still differ by 30–60 dB at harmonics 20–50, and differ again on transients, intermodulation and level-dependence. Fit to the tail, to several input levels and to bursts, not just total THD. [Computed; Inference]
4. **Sharp knees and bias make the timbre "lumpy" with drive.** Individual harmonics pass through deep nulls at particular drive levels, and those nulls move up the harmonic series as drive rises. A symmetric tanh has none, but **a biased tanh or a biased soft curve has them too**. While drive moves, the null sweeps and can be heard as a narrow notch moving up. [Computed; Cook 2020]
5. **Real analog gear has memory.** Coupling-cap charging from grid current (bias shift, "blocking"), cathode-bypass caps, supply sag and magnetic hysteresis make the curve depend on recent level. Attacks and sustains hit different curves, which steady sines cannot reveal. The cheap model is an asymmetric curve whose bias follows a one-pole envelope (tens of ms), then a DC blocker. [Documented: Pakarinen & Yeh 2009; Aiken; Chowdhury 2019]
6. **Aliasing grows with drive and knee sharpness.** Harmonics above Nyquist fold back as inharmonic tones that move against the pitch. With a slow (1/f-like) spectrum, each doubling of oversampling buys only about 6 dB. First-order ADAA plus 2–4x oversampling is the strongest practical recipe; polyBLAMP suits true hard clippers. [Documented: Vicanek 2023/24; Esqueda et al. 2016; Parker et al. 2016]
7. **Oversampling filters have side effects.** JUCE's FIR mode is linear-phase but has more latency; the IIR mode has less latency but phase shift near the top. Report latency, and delay every dry path (Mix, Delta, Clean Low) by the same amount, or parallel paths comb-filter. [Documented: JUCE source]
8. **A stepped parameter is amplitude modulation.** Updating gain once per block or event at rate fu puts sidebands at ±k·fu around every component, and through a nonlinearity around every harmonic, with the same Hz spacing. Smooth per sample, ideally in dB, over at least the gap between incoming updates. [Documented: JUCE docs; Inference for the maths]
9. **Anything that depends on Drive should use the same smoothed value.** That includes gain, curve settings and auto gain. Otherwise the pieces move at different times and modulate the sound. [Inference]
10. **Moving filters need modulation-safe structures.** Use TPT/zero-delay-feedback or state-variable filters (Zavalishin; Wishnick DAFx-14), not direct-form biquads, and avoid crossfading or copying filter state between settings (Adriaensen; Goetze).
11. **Drive implemented as a gain into a fixed curve has no coefficient-update artifacts.** Circuit models (DK/WDF) must recompute matrices when a pot moves, so they need smoothing plus sub-block updates. [Documented: Holters & Zölzer 2011; Chowdhury 2022]
12. **Auto gain is approximate by nature.** Prefer static compensation computed from Drive/Character (RMS of a reference signal through the curve), optionally trimmed by a slow (1–3 s) RMS follower, over a fast follower that would pump. Decapitator's Auto is a gain tied to Drive, not a loudness meter. [Documented: Decapitator manual; FabFilter help; Inference]

## How to work with this knowledge

- **Diagnose before changing.** Turn a complaint ("less gritty", "fizz", "phaser while turning") into two or three competing hypotheses, then pick the measurement from `measurement-cookbook.md` that separates them. Each hypothesis should predict a different result.
- **Measure at the real level.** Rubbish's earlier fits were done at −22 dBFS while the plugin actually saw about −13 dBFS in Logic. Distortion is level-dependent, so always state and match the input level.
- **Match outputs before listening.** Turn Auto Gain/Auto off or account for it; louder always sounds "better" or "grittier".
- **Prefer the cheapest change that tests the idea:** an offline render with changed numbers before a code-structure change, and a code-structure change before a new feature. Ask the owner before anything prd.md doesn't cover (CLAUDE.md).
- **Real-time rules still apply** (CLAUDE.md): no allocation, locks or blocking in `processBlock()`; `juce::SmoothedValue` on every audio parameter; report oversampling latency; safety limiter last; filters stable at all settings.

## Rubbish map (where this knowledge lands in the code, as of 2026-11-27)

- `Source/dsp/Waveshapers.cpp` holds the Character curve: soft clip x/(1+|x|^h)^(1/h), with a bias capped at a fraction of the tracked signal peak, the centre offset subtracted, and normalisation.
- `Source/dsp/CharacterShaper.cpp` handles Drive/Push glides (50 ms `SmoothedValue`s), recomputes curve settings every 16 samples at the oversampled rate, crossfades Characters over 30 ms, and runs the peak follower that limits the bias (0.1 s release).
- `Source/dsp/CharacterVoicing.cpp` holds the per-Character numbers (sensitivity, bias terms, hardness, Push terms, bias limit).
- `Source/dsp/DistortionStage.cpp` applies drive gain (50 ms ramp), then `juce::dsp::Oversampling` (2 stages = 4x, FIR equiripple, max quality, integer latency), then shaper and Spread, then downsampling and a first-order 10 Hz DC-blocking high-pass.
- Fit history and P measurements are in `decapitator-analysis.md` and `measurements/*.csv`. Reference recordings are in `~/Desktop/rubbish Test files/`.

Verify these details against the code before relying on them; they will drift.
