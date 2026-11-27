# Harmonics, curves and stateful distortion

Contents: 1. Curve smoothness → upper-harmonic tail · 2. Symmetry, bias and even harmonics · 3. Harmonic nulls vs drive · 4. Chebyshev / designing a harmonic recipe · 5. Stateful distortion (bias shift, blocking, sag, hysteresis, tape) · 6. Intermodulation and pre/post emphasis · 7. DC handling · 8. Auto gain · 9. Practical implications for Rubbish

Labels: [Documented] cited source · [Computed] numerical check in this project's research (pure-tone FFT, no aliasing) · [Inference] · [Uncertain].

---

## 1. Curve smoothness sets the upper-harmonic tail

**Rule [Documented: general Fourier theory, e.g. UC Davis/Marquette course notes; no audio paper found that states it for waveshapers]:** for a periodic waveform, a jump gives coefficients ~1/n, a kink (continuous, slope jumps) gives ~1/n², and each extra continuous derivative adds another power of 1/n. Infinitely smooth (analytic) shapes decay exponentially.

Applied to a sine through a curve whose (k+1)-th derivative jumps at the knee, harmonics decay about **−6·(k+2) dB/octave** at high n:

| Curve | Smoothness | Tail slope [Computed] |
|---|---|---|
| Hard clip | C0 (slope jumps) | ≈ −12 dB/oct |
| Cubic soft clip 1.5(x − x³/3) | C1 | ≈ −18 dB/oct |
| Quintic soft clip | C2 | ≈ −24 dB/oct |
| tanh, atan, x/√(1+x²) | analytic | accelerating (tanh at drive 4: −53, −107, −112 dB/oct over successive octaves) |
| x/(1+\|x\|^8)^(1/8) (sharp knee) | analytic for even h | accelerating, but much slower than tanh (−18, −40, −53, −118 dB/oct) |

- **Heavy drive caveat [Computed]:** a hard-clipped sine driven hard looks like a square wave, so the low harmonics follow roughly 1/n (−6 dB/oct) up to a crossover set by the length of the sloped part, then −12 dB/oct. So "−12 dB/oct" is the asymptote, not the slope everywhere. This is why Vicanek treats a hard-driven saturator as a 1/f spectrum when sizing oversampling (see `aliasing.md`).
- **Drive moves the tail [Computed; Inference]:** raising drive on an analytic curve makes the knee sharper relative to the signal, so the tail slows and the curve sounds more like hard clipping over the audible band.
- **Equal drive, different curves [Computed]:** at drive 4, H3/H9/H27/H81 relative to the fundamental are hard clip −10/−28/−51/−65 dB and tanh −11/−32/−92/−272 dB. The low harmonics barely differ; the tail differs enormously.
- **Perception [Documented: Vallejo Reyes, AES LatAm 2024]:** a hard clipper's abrupt transition is heard as harsher and more piercing. The constant c in tanh(c·x)/tanh(c) moves the character from soft to hard.
- **Rubbish's family [Inference]:** x/(1+|x|^h)^(1/h) sweeps from soft (h≈1–2) to near-hard (h→∞) with one number. Large h behaves like hard clip up to high harmonics: more "bite", and more need for oversampling.

**Why it matters for matching:** the 2026-11-26 study found P's tail falls at a steady rate, like a curve with a definite bend point, while Grit's very smooth curve kept the tail tiny until driven hard. That fits this rule. A small amount of curve "corner" changes the tail far more than any H2–H5 fit can show.

## 2. Symmetry, bias and even harmonics

- **[Documented: Pakarinen & Yeh 2009]** A symmetric static nonlinearity produces only odd harmonics. An asymmetric one (their example is from the Doidic et al. Line 6 patent) produces both odd and even. Pritchard (1991) used an asymmetric first stage specifically to get even harmonics.
- **[Documented: Cook 2020]** For symmetric hard clipping, the even coefficients are exactly zero, H3 dominates, and H5's share rises with a "bump" rather than smoothly.
- **[Documented: Klanghelm IVGI2 manual, via search summary]** IVGI's asymmetry keeps the negative half cleaner and adds even harmonics; it is described as preserving dynamics.
- **Fixed-bias behaviour [Computed]**, y = tanh(g·s + 0.3) − tanh(0.3):

| g | H2 | H3 | H4 | DC / fundamental |
|---|---|---|---|---|
| 0.1 | −37 | −64 | −94 | −0.015 |
| 0.5 | −24 | −36 | −53 | −0.069 |
| 1 | −20 | −25 | −38 | −0.119 |
| 2 | −19 | −16 | −28 | −0.167 |
| 4 | −23 | −12 | −26 | −0.196 |
| 10 | −31 | −10 | −31 | −0.215 |

  H2 leads at low drive, H3 takes over around g ≈ 1.5–2, and H2 falls back at heavy drive while DC keeps growing. To keep a strong, lopsided H2 at the top (as P does: H2 only 4–6 dB under the note at Drive 7–10), the bias must scale with drive or level.
- **Bias capped by a peak follower (Rubbish) [Inference]:** capping bias at a fraction of the signal's own peak keeps quiet signals from collapsing to silence. But if the follower releases quickly, the cap moves within one slow cycle, so the curve wobbles once per cycle. That adds extra harmonics, which is the low-Drive buzz noted 2026-11-26.

## 3. Harmonic nulls vs drive (the "lumpy" timbre)

[Computed in this project, 2026-11-27, 4096-point single-cycle DFT, pure sine; a "null" means a harmonic ≥10 dB below both neighbours of the same parity]

| Curve | Bias | Nulls (harmonic numbers) at drive 1.5 / 2 / 3 / 4 / 6 / 8 × knee |
|---|---|---|
| Hard clip | 0 | 9,13,17 / – / 19,37 / 13,25,37 / 19 / 25 |
| tanh | 0 | none at any drive |
| x/(1+\|x\|^1.6)^(1/1.6) | 0 | none |
| x/(1+\|x\|^8)^(1/8) | 0 | – / 13,33 / 19 / 13,39 / 19,39 / – |
| Hard clip | 0.3 | 22,39 / 6,9,12 / 13,15 / 17,19,21 / 4,19,27 / 4,6,25 |
| tanh | 0.3 | 11,22 / 13,26 / 17,19,34 / 21,23,25 / 29,31,33 / 37,39 |
| x/(1+\|x\|^1.6)^(1/1.6) | 0.3 | 9,16 / 11,20 / 15,17 / 19,21,23 / 27,29,31 / 35,37 |

What it shows:
- **Hard or sharp knees** put nulls in individual odd harmonics. The first null moves up roughly with drive (for hard clip about n ≈ 3 × drive; see `parameter-changes.md`). For a symmetric hard clip, H5 vanishes at about 1.65× the clip level (+4.3 dB), H7 at about 1.26× and 2.26×, and H9 at about 1.15×, 1.55× and 2.9×. These are trigonometric functions of arcsin(1/g), not Bessel functions [Inference].
- **Smooth symmetric curves** (tanh, soft h=1.6) have **no** nulls: every harmonic rises steadily.
- **Any bias** gives nulls even on smooth curves, because the even and odd contributions interfere. With bias, the null region also **climbs steadily with drive** (tanh: 11 → 13 → 17 → 21 → 29 → 37). Low even harmonics (H4, H6) can also drop out at high drive.
- Decapitator's own sweeps show this: harmonics briefly vanish at certain Drive positions (A: H2 near Drive 9; T: H2 near 3.5; P: H2 collapses to −37 dB around Drive 4). See `decapitator-analysis.md`.
- **Audibility [Inference]:** a static null sits on one harmonic and is hard to notice. A null that moves while drive moves becomes a narrow notch sweeping through the harmonic series, which is audible, especially where the tail is loud.

## 4. Designing a harmonic recipe (Chebyshev and level-switching)

- **[Documented: Le Brun 1979, via Puckette, CMU notes and Hayes et al. 2023]** A full-scale sine through the Chebyshev polynomial T_k gives exactly the k-th harmonic, so a weighted sum gives any harmonic recipe, but **only at input amplitude 1**. At other levels the spectrum changes.
- **[Documented: Pakarinen & Yeh on Fernández-Cid & Quirós 2001]** They used per-band Chebyshev shapers, omitted high harmonics to avoid aliasing, and crossfaded two polynomials by band level for level-dependence.
- **[Documented: Pakarinen & Yeh on Gustafsson et al. 2004, Softube]** Polynomial coefficients are switched by signal energy, and different sets are used for rising and falling input to imitate hysteresis.
- **[Inference]** To match a reference across levels, fit the curve's parameters to measurements at several levels (what Rubbish does). A Chebyshev expansion fitted at one level only matches at that level.

## 5. Stateful distortion: why real gear isn't a static curve

**Tube mechanisms [Documented: Pakarinen & Yeh 2009]:**
- Capacitances make tube stages not memoryless, so a static curve can't model them accurately.
- Grid conduction on big transients charges the coupling cap, shifting the bias. The result is "dynamically varying transient distortion" and, in the extreme, blocking.
- The cathode-bypass cap holds a memory of the bias, so distortion depends on recent history.
- Miller capacitance low-passes the input to the stage.
- Class-AB supply sag adds programme-dependent compression.
- The output transformer has hysteresis: rising and falling signals distort differently.

**Blocking [Documented: Aiken, "What is blocking distortion?"]:**
- Past zero bias the grid conducts like a diode. The AC-coupled driver can't feed it, so peaks clamp and the waveform shifts down as drive rises.
- A low, loud transient can cut the stage off until the coupling RC recovers. The result is a choppy, "farting" sound plus a crossover dead zone.
- Time constants from the page's component values are roughly 2–22 ms [Computed by researcher].

**Real-time models reviewed by Pakarinen & Yeh:**

| Model | Approach |
|---|---|
| Kuroki & Ito 1998 (Yamaha) | Envelope-shifted input offset into a lookup table. Because of these history-dependent offsets, cascaded stages can't be merged into one table. |
| Karjalainen et al. 2006 | Measured curve plus grid current and bias in a feedback loop, a low-pass at the grid and a high-pass between stages. |
| Gallo 2008 | Cathode-voltage ODE solved with RK4. |
| Yeh & Smith 2008 | State-space with a static multidimensional nonlinearity in the loop. |
| Kemp 2006 (dynamic convolution) | Level-indexed impulse responses. Fails for dynamic nonlinearities whose curve shape changes (Berners & Abel 2004). |

**Tape [Documented: Chowdhury, DAFx-19]:**
- Jiles–Atherton hysteresis with a high-frequency bias signal: 55 kHz at about 5× the input for the Sony TC-260.
- RK4 solver; the trapezoidal rule went unstable at high frequencies.
- Needs 16× oversampling.
- Too little bias gives a zero-crossing dead zone; too much gives more saturation. Silence through the model produces realistic hiss.

**What stateful elements add to "grit" [Inference; no controlled listening study found]:**
- Attacks hit a less-biased curve than sustains.
- After loud hits, even harmonics and DC drift and then recover.
- Sag gives a "bloom" of compression.
- Blocking chokes the sound.

None of this shows in a steady-sine THD sweep; it needs burst tests (see `measurement-cookbook.md`).

**Cheapest convincing dynamic model [Inference]:** asymmetric curve → input bias driven by a one-pole envelope (attack and release in the 5–50 ms range) → DC blocker. Add sag as an envelope-driven threshold reduction. Note that prd.md shelves "Touch (louder input = more distortion)", so a dynamic bias that audibly makes loud notes dirtier needs the owner's decision.

## 6. Intermodulation and pre/post emphasis

- **[Documented: Pakarinen & Yeh 2009]** Broadband input through one shaper creates intermodulation (sum and difference tones), which is perceptually disturbing. Splitting into bands before distorting reduces it. Sonnox Inflator's Band Split exists for exactly this reason, although its manual says unsplit usually sounds nicer.
- **[Documented: McGill MUMT-307 notes, snippet]** Filter order matters because nonlinearities don't commute with filters.
- **[Documented: KVR thread with Sean Costello, snippet; US patent 4306256, snippet]** Pre-emphasis → clip → de-emphasis is a standard trick. A matched pair restores the tone only approximately, because clipping products survive the de-emphasis.
- **[Documented: Decapitator manual]** Low Cut, Thump and Tone act before the saturation; High Cut/Steep after. Saturn's Tone acts on the processed signal. Klanghelm's "Response" shifts which frequencies saturate most.
- **Project finding (2026-11-26):** P shapes notes at 44/175/700/2800 Hz the same way (with Tone centred), so there is no hidden fixed emphasis.

## 7. DC handling

- Asymmetric shaping creates DC that **grows with drive** (table in §2). Subtracting f(bias) removes only the static part; the level-dependent part needs a high-pass. [Computed]
- **[Documented: Smith, "DC Blocker", CCRMA]** y[n] = x[n] − x[n−1] + R·y[n−1], with R ≈ 0.995 at 44.1 kHz (lower R means a higher cutoff).
- **[Inference]** DC-block after each asymmetric stage and before any later nonlinearity, unless the bias shift into the next stage is the intended effect (tube-like interaction). A DC blocker with a cutoff in the few-Hz range also sets how fast the "bias recovery" after a loud hit sounds.

## 8. Auto gain / loudness compensation

- **[Documented: FabFilter forum, snippet]** Saturn compensates on RMS, not peak, because saturation reshapes transients. Compensation can't be exact for a nonlinear process.
- **[Documented: Decapitator manual]** With Auto on, the Output knob moves opposite to Drive. The 2026-11-25 measurements found it is not level-exact (error −3 to −10 dB depending on style).
- **[Documented: practitioner sources, KVR/JUCE forum/GainMatch, snippets]** RMS or LUFS matching over about 3–5 s, or static parameter-driven compensation.
- **[Inference]** Best fit for Rubbish: compensation precomputed from Drive, Push and Character, driven by the same smoothed Drive value as the gain, plus at most a slow trim. A fast follower would pump and would fight Dynamics Restore.

## 9. Practical implications for Rubbish (inferences)

- To get P's brighter tail at mid Drive, give the curve a defined bend (higher h, or a C1/C2 piecewise knee) rather than only refitting bias numbers. Expect to need more oversampling, or ADAA.
- To get P's lopsided H2 at the top, the bias must grow with drive or level faster than the signal swamps it.
- If the owner wants "lumpy" character movement while turning Drive (P's turnover zone, notches), a sharper knee and/or bias that changes with drive produces it naturally. A smooth symmetric curve never will.
- Don't judge grit on single sines only; add bursts and two-tone tests.
