# What happens while a parameter moves

Contents: 1. Stepped parameters = amplitude modulation (zipper) · 2. How hosts deliver automation · 3. Smoothing methods · 4. Filters under modulation · 5. Mechanisms for a moving notch or comb · 6. Discriminating tests · 7. Rubbish implementation notes

---

## 1. Stepped parameters produce sidebands

- **[Documented: Steinberg forum; ChucK users list 2007; Eventide forum]** Without interpolation a gain moves as a staircase, and the steps carry high-frequency content (zipper noise). The artifact's frequency follows the update interval. The fix is ramping on the audio thread.
- **Maths [Inference; no published derivation found]:**
  - A stepped value is a smooth ramp plus a periodic error e(t) at the update rate fu, roughly a sawtooth with energy at k·fu falling as 1/k.
  - Multiplying a tone at f by it adds lines at f ± k·fu.
  - The first sideband pair sits at about (step size / value) / π relative to the carrier, so faster knob moves give bigger steps and louder sidebands. A still knob gives none.
- **Through a nonlinearity [Inference]:**
  - Each harmonic amplitude A_n depends on drive, so stepping drive modulates every A_n at fu.
  - Every harmonic n·f0 gets sidebands at n·f0 ± k·fu, with **the same spacing in Hz**. That constant spacing is the fingerprint of AM; FM/vibrato would scale with n.
  - High harmonics change fastest with drive, so their sidebands are relatively deeper.
- **Expected spacings if the step is the audio block** (fu = fs / block):

| Block | 44.1 kHz | 48 kHz |
|---|---|---|
| 4096 | 10.8 Hz | 11.7 Hz |
| 5120 | 8.6 Hz | 9.4 Hz |
| 6144 | 7.2 Hz | 7.8 Hz |
| 8192 | 5.4 Hz | 5.9 Hz |

  7–10 Hz implies blocks of about 4,400–6,900 samples, which is unusual. A UI, automation-event or internal timer rate of about 100–140 ms is a likelier source [Inference].

## 2. How hosts deliver automation (mostly anecdotal)

- **Logic [Documented: Apple Logic Pro manuals]:**
  - Logic offers sample-accurate plug-in automation, which can be limited or turned off in Preferences → Audio → General ("Sample Accurate Automation").
  - Not all AUs support it.
  - Timing can be affected by latency and CPU load.
- **Logic and JUCE AUs [Documented, anecdotal: JUCE forum threads]:**
  - Logic is reported to split buffers rather than send AU ramps (REAPER does send ramps).
  - It may give AUs a fixed block size such as 1024, adjustable through "Process Buffer Range", and smaller blocks while automating.
  - JUCE reads parameters once per `processBlock`.
  - Logic is reported to accept only about 4 plugin→host parameter changes per second when a plugin records automation.
- **VST3 [Documented: Steinberg dev portal]:** each parameter arrives as a queue of (sampleOffset, value) points, with a straight line implied between them. Steinberg's own example updates gain every 8 samples. Renoise says many plugins ignore the offsets.
- **AU [Documented: Apple AudioUnitParameterEvent]:** `kParameterEvent_Ramped` gives start value, end value and duration in frames, and requires `kAudioUnitParameterFlag_CanRamp`.

## 3. Smoothing methods

- **[Documented: JUCE docs]** `juce::SmoothedValue`:
  - `Linear` steps by equal increments; `Multiplicative` steps by equal ratios, so it is linear in dB or pitch and can't reach 0.
  - `reset(sampleRate, seconds)`, `setTargetValue`, `getNextValue`, `skip(n)`.
  - The ramp restarts from the current value whenever a new target arrives.
- **[Documented: JUCE forum]** The ramp speed of Multiplicative mode depends on the start and end values.
- **Choosing the time [Inference]:**
  - A linear ramp that finishes before the next update leaves flat segments between ramps, which is still a weak modulation at the update rate (corners instead of steps).
  - Make the ramp at least as long as the update interval, or use a one-pole low-pass smoother with a time constant at least that long (50–100 ms if updates arrive at about 10 Hz).
  - Smooth gain-like parameters in dB.
- **Use one smoothed value per control [Inference]:** drive gain, curve settings and auto-gain compensation must all read the same smoothed Drive, sample by sample, or they move at different moments and produce AM.
- **Block-rate recompute of expensive settings is fine** if the inputs are smoothed and the recompute interval is short compared with the glide. Rubbish recomputes curve settings every 16 oversampled samples.

## 4. Filters under modulation

- **[Documented: Zavalishin, *The Art of VA Filter Design* 2.1.0, §7.12; "KeepTopology" paper]** Topology-preserving (trapezoidal, zero-delay-feedback) filters behave well when time-varying. Transform-derived direct forms show strong artifacts under modulation.
- **[Documented: Laroche, JAES 2007]** Some filter topologies stay stable when their coefficients change; he gives sufficient criteria and ways to stabilise others. Werner & McClellan (DAFx 2022) extend the criterion.
- **[Documented: Wishnick, DAFx-14]** Recommends the state-variable filter, with a proof that it stays stable when time-varying; listening tests agreed.
- **[Documented: Adriaensen; Goetze, linux-audio-dev 2013]** Prefer smoothing parameters to crossfading two static filters; crossfades depend on the two filters' phase difference. Copying old biquad state into new coefficients can spike.
- **[Documented: KVR]** Direct-form filters blew up under fast cutoff modulation above about 2.5 kHz in one user's report.

## 5. Mechanisms that can make a narrow notch or comb move while Drive moves

| # | Mechanism | Present while knob still? | Signature |
|---|---|---|---|
| A | **The curve's own harmonic nulls** (`harmonics-and-curves.md` §3): with a sharp knee or any bias, particular harmonics vanish at particular drives, and the null climbs the harmonic series as drive rises (hard clip: first null at about n ≈ 3 × drive) | Yes, but parked on one harmonic, so it is hard to hear | Only with tonal input. Steady renders at fixed Drive values show the null moving between harmonics. Pink noise shows no notch. |
| B | **Dry + wet paths out of alignment** (Mix < 100%, parallel bands, latency mismatch, IIR oversampler phase). [Documented: JUCE forum dry/wet latency threads; iZotope comb filtering] | Yes, if the delay or phase is fixed | Regularly spaced notches (comb) on noise too. It moves only if the delay or phase depends on drive (drive-dependent emphasis or bias filter). |
| C | **Crossfading between two settings or instances** on each parameter change [Documented in general: SMC 2024; Adriaensen] | No | Transient cancellation during each crossfade. Repeated crossfades at a fixed rate add AM sidebands at that rate. No evidence any named plugin does this. |
| D | **A tone filter whose cutoff tracks drive** | Yes (static EQ at a fixed drive) | A peak or notch at a drive-dependent frequency; visible in noise. |
| E | **Auto output with lag or steps** | No | Sidebands, not a notch. Changes with Auto on/off. |
| F | **Aliases of a moving null** [Inference] | As A | A null climbing above Nyquist folds back and appears to sweep **down** among the aliased lines. |
| G | **Stepped drive** (§1) | No | Same Hz sideband spacing around every harmonic. Beating sidebands can be heard as roughness or a "phasey" wobble. |

Several can act at once. In the P recordings, (A) or (D) could explain a moving notch, and (G) or (E) the 7–10 Hz sidebands.

## 6. Tests that separate the causes

1. **Sideband spacing vs block size and sample rate.** Use a 220 Hz sine through the plugin and automate Drive with a linear ramp. Vary the session rate (44.1/48 kHz), I/O buffer (128/1024), Process Buffer Range and Sample Accurate Automation.
   - Spacing = fs / block → block-rate stepping.
   - Spacing fixed whatever you change → internal timer.
   - Spacing follows ramp steepness → value quantisation.
2. **Ramp speed** (1 s / 4 s / 16 s for the same sweep). Stepping makes sideband *level* scale with slope.
3. **Hand-turned vs drawn automation.** Different spacing points to UI or event rate.
4. **Auto on vs off** (fixed Output). Sidebands collapse with Auto off → (E).
5. **Mix 100% vs 50%.** A notch only below 100% → (B) or (C).
6. **Drive map, no automation:** render or record steady sines at fixed Drive steps of 0.1–0.25 and plot H1–H40. A null that moves between harmonics as Drive steps → (A). This needs no time variation.
7. **Pink noise with automated Drive** (spectrogram). A moving comb or notch → B/C/D. Nothing → points to A (a memoryless curve on noise has no harmonic nulls).
8. **Repeatability null test:** two identical automated renders, one polarity-inverted, should cancel. Then null automated vs static-at-endpoint to isolate the transition.
9. **Aliasing check:** a high sine at fixed Drive, run at 44.1 vs 96 kHz. Aliases move or vanish; real harmonics don't.

## 7. Rubbish implementation notes (verify in code)

- Drive gain ramp: `DistortionStage` (`driveGain.setRampDurationSeconds(driveRampSeconds)`). Curve settings follow 50 ms `SmoothedValue`s of driveDb/totalDb/push in `CharacterShaper` and are recomputed every 16 samples at the oversampled rate. Character changes crossfade over 30 ms.
- Rubbish therefore has none of mechanisms C, E or G by design. Whether it shows A depends on its curve: Grit is biased, so in theory it has moving nulls; whether they are deep and loud enough to hear is untested (see `rubbish-grit-vs-p.md`).
- LFOs moving Drive (v3) make every mechanism above relevant at LFO rate. Keep LFO modulation per sample (or per short sub-block) and smoothed, and keep all Drive-dependent pieces on the same modulated value.
