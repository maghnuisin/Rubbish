# Aliasing and antialiasing

Contents: 1. Why distortion aliases · 2. How much oversampling · 3. juce::dsp::Oversampling in detail · 4. ADAA · 5. polyBLAMP · 6. Other methods · 7. Audibility · 8. Aliasing as "character" · 9. Recommendations for Rubbish

---

## 1. Why distortion aliases

- **[Documented: Parker, Zavalishin & Le Bivic, DAFx-16; Vicanek 2023/24]** Applying a curve sample-by-sample samples the distorted continuous signal directly, so harmonics above Nyquist (fs/2) fold back as inharmonic tones.
- **[Documented: standard sampling theory; Sound On Sound, Dec 2023]** A component at f > fs/2 appears at |f − k·fs|. The first fold lands at fs − f, so as the pitch rises the alias falls. The folded tones are not multiples of the note, so they sound noisy or out of tune.
- **[Documented: Holters, DAFx-19]** Aliasing is "usually perceived as unpleasant". When the harmonics decay slowly, plain oversampling needs high factors.
- **Project evidence (2026-11-27):** a 2800 Hz note through P at Drive 10 shows tones at 22,855, 20,062 and 17,268 Hz, which are harmonics 9–11 folded around 24 kHz (48 kHz session). The 44 Hz sine shows none, because its harmonics die out before Nyquist. So P doesn't oversample enough to prevent this.

## 2. How much oversampling a curve needs

- **[Documented: Vicanek]** The benefit depends on how far the spectrum extends past Nyquist and how fast it falls. For a 1/f spectrum (hard-driven saturator), 2x cuts aliasing by only about 6 dB, 4x by about 12 dB.
- **[Documented: Esqueda, Bilbao & Välimäki, IEEE TSP 2016]** For hard-clipped sines and triangles, 2x oversampling improved SNR by about 9.2–9.5 dB on average.
- **[Documented: Chowdhury, ADAA notebook]** A 1244.5 Hz sine at gain 10 through a hard clipper at 48 kHz: 4x helps a lot but aliasing is still visible. 4x costs about 4x the CPU.
- **[Documented: FabFilter Saturn 2 help, via search summary]** HQ "Good" = 8x ("more than sufficient in most cases"); "Superb" = 32x, drastic CPU.
- **[Documented: Kazrog KClip 3 guide]** 2x–32x selectable, but the final Ceiling hard clip runs at the session rate with no oversampling. KClip Zero uses 2x for real-time and 16x for offline rendering.
- **[Inference, from the knee rule in `harmonics-and-curves.md`]** Smooth curves at moderate drive need the least oversampling; sharp knees, large h and heavy drive need the most. Choose per Character, or for the worst one.

## 3. `juce::dsp::Oversampling` (read from JUCE source, `juce_Oversampling.cpp`, master, Nov 2026)

- **Signature:** `Oversampling(numChannels, factor, filterType, isMaxQuality, useIntegerLatency)`. `factor` is the number of cascaded 2x stages (<5, so 2x–16x; 0 = pass-through).
- **Filter types:**
  - `filterHalfBandFIREquiripple`: linear phase, more latency.
  - `filterHalfBandPolyphaseIIR`: polyphase allpass, lower latency, with phase shift growing towards the top of the band.
  - A source comment calls the IIR version "minimum phase"; the class author says on the JUCE forum that this is a misconception and that it still has latency.
- **Per-stage targets** (normalised transition width up/down, stopband up/down), the same for both types:

| isMaxQuality | Stage 0 (first 2x) | Stage 1 | Stage 2 | Stage 3 |
|---|---|---|---|---|
| true | 0.05/0.06, −90/−75 dB | 0.10/0.12, −80/−65 | 0.10/0.12, −70/−55 | 0.10/0.12, −60/−45 |
| false | 0.06/0.075, −70/−60 dB | 0.12/0.15, −62/−52 | 0.12/0.15, −54/−44 | 0.12/0.15, −46/−36 |

  Note the downsampling stopband in stage 0 is −75 dB even at max quality. Aliases from the oversampled shaper that land in the top transition band are attenuated less.
- **Latency:**
  - FIR stage: (up order + down order)/2, at that stage's rate.
  - IIR stage: phase delay near DC.
  - The stages are scaled to base-rate samples and summed.
  - `useIntegerLatency = true` pads with a fractional delay so `getLatencyInSamples()` returns a whole number. No published table exists, so print it in `prepareToPlay()`.
- **Practical:**
  - Report the latency to the host (`setLatencySamples`).
  - Delay every dry or parallel path (Mix, Delta, Clean Low recombination, Dynamics Restore's reference) by the same amount, or they comb-filter.
  - The IIR's phase shift near the top means a fixed delay can't fully align dry and wet up there.
  - **[Documented: Sound On Sound 2023]** Oversampling side effects: CPU, latency (sometimes misreported), phase issues in parallel setups, higher true peaks, top-end roll-off from stacked gentle filters. Oversampling does **not** change intermodulation distortion that lands below Nyquist.

## 4. Antiderivative antialiasing (ADAA)

- **First order [Documented: Parker et al. 2016; Bilbao, Esqueda, Parker & Välimäki, IEEE SPL 2017]:** y[n] = (F1(x[n]) − F1(x[n−1])) / (x[n] − x[n−1]), where F1 is the curve's antiderivative. This is equivalent to continuous-time convolution of the distorted signal with a rectangular kernel.
- **Ill-conditioning [Documented: Chowdhury notebook; Vicanek]:** when x[n] ≈ x[n−1], use f((x[n]+x[n−1])/2) instead. The threshold is 1e-5 in Chowdhury's code, or about √ε per Vicanek. Second order has more divisions and needs several fallbacks.
- **Side effects [Documented: Holters DAFx-19]:**
  - For f(u) = u it reduces to ½(u[n]+u[n−1]): half a sample of delay plus a gentle low-pass with a zero at Nyquist.
  - Modest oversampling (2x) counters the low-pass.
  - The delay matters inside feedback loops.
- **Vicanek's variant [Documented]:** one sample of total delay, with an identity response of (x[n]+6x[n−1]+x[n−2])/8, only −6 dB at Nyquist. Pre/post one-pole compensation makes it transparent in the linear region. He also recommends x/√(1+x²) as a tanh-like curve whose ADAA is numerically stable everywhere.
- **Where it works [Documented: Vicanek]:** the kernel is a sinc with zeros at multiples of fs, so it suppresses aliases that land low in the spectrum best and high ones poorly. **ADAA + 4x oversampling "makes aliasing practically a non-issue even for a hard clipper."**
- **Higher order [Documented: Bilbao et al. 2017; WASPAA 2017 via Vicanek]:** better SNR, but more in-band droop and more oversampling to compensate.
- **Stateful systems [Documented: Holters, DAFx-19 / Applied Sciences 2020]:** ADAA's delay breaks feedback loops. Holters folds the antialiasing into the state update. Follow-ups include La Pastina & D'Angelo (frequency compensation), Holters 2022 (combined derivative/antiderivative), Zheleznov & Bilbao DAFx24 (cubic kernels) and Albertini, Bernardini & Sarti (ADAA in WDFs, DAFx-20); all cited from titles or snippets.
- **Cost:** a closed-form F1 (and F2 for second order) is needed for every curve. Rubbish's x/(1+|x|^h)^(1/h) with a non-integer, drive-dependent h has no simple closed-form antiderivative [Inference]. It would need a tabulated F1 or a curve family with known integrals.

## 5. polyBLAMP (hard corners only)

- **[Documented: Esqueda, Bilbao & Välimäki, IEEE TSP 2016; "Rounding Corners with BLAMP", DAFx-16]**
  - Detect each clipping point, estimate its fractional position, and add a polynomial band-limited-ramp correction to the neighbouring samples.
  - Two-point and four-point versions improve SNR by about 12 and 20 dB, at less cost and latency than oversampling.
  - The methods beat low-factor oversampling for fundamentals below about 3 kHz, and the output stays inside the clip threshold.
  - A related paper applies it to soft clipping.
- **[Inference]** Use it only for true clippers or folders with locatable corners; ADAA is the general tool.

## 6. Other methods

- **[Documented: Carson, Wright & Bilbao, DAFx25, arXiv 2505.11375]** Fine-tuning neural amp models on sines with inharmonic content removed beats 2x oversampling in most cases, but it changes the harmonics.
- **[Documented: Eventide/Newfangled "Saturate" marketing]** Claims an antialiasing mode better than oversampling; this is vendor marketing only.

## 7. Audibility

- **[Documented: Lehtonen, Pekonen & Välimäki, JASA 2012]** Masking dominates. It is stronger above the fundamental than below, and they propose an A-weighted noise-to-mask ratio (developed for oscillators).
- **[Documented: Schimmel, Radioengineering 2012]** With enough oversampling, aliases get masked by the harmonics.
- **[Documented: Esqueda et al. 2016]** Aliasing matters less for fundamentals below about 400 Hz.
- **[Documented: Sound On Sound 2023]** In a single plugin it is often inaudible but it accumulates across a chain. Visible on a spectrogram isn't the same as audible.
- **[Inference]** It is most audible on high, pure, sustained tones and on pitch movement (vibrato, glides, detuned unison), where the inharmonic lines move against the notes. Moving aliases can sound like a phaser or pitch wobble. Low, dense bass masks it.

## 8. Aliasing as deliberate "character"

- **[Documented: Sound On Sound 2023]** Some plugins' character includes aliasing, especially digital-distortion emulations: "if it sounds better without oversampling, accept it."
- **No major saturator maker was found documenting aliasing as a designed feature.** FabFilter, Kazrog and others present oversampling as a quality switch.
- **[Documented, low reliability, KVR]** Users report Decapitator aliasing at 44.1 kHz that improves at 96 kHz; another user's test ranked it least aliasing of four. Soundtoys says nothing about it.
- **[Documented, author opinion: Airwindows]** Chris Johnson argues un-oversampled saturation can keep overtones clear by smoothing or band-limiting; forum users dispute this.
- **For Rubbish:** prd.md requires "no audible aliasing at any Drive setting" and excludes user-adjustable oversampling. Any controlled-aliasing idea is the owner's decision and needs a prd change first.

## 9. Recommendations for Rubbish (inferences)

- Keep oversampling on the distortion stage. Check `getLatencyInSamples()` and that every dry path is delayed by it.
- If a sharper knee is adopted for more grit, re-check aliasing with a high sine (5–8 kHz) at full Drive + Push. Consider 4x → 8x, or ADAA on a curve with a closed-form antiderivative.
- The MS-20-style filters with Peak near self-oscillation and Push on are a nonlinear feedback loop. Memoryless ADAA would detune them; oversample the loop instead, or use Holters' stateful approach.
