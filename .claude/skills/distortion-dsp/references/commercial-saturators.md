# Commercial saturators: what their makers document

Only what is publicly documented or reported. Treat makers' internal designs as unknown unless stated. [Documented] = manual or help page; [Third-party] = interview, review or forum; [Inference].

## Soundtoys Decapitator (User's Guide v5, 2015; product page)

- **Styles [Documented]:**
  - **A:** Ampex 350 tape preamp; smooth, tube-like, lots of gain.
  - **E:** Chandler/EMI TG channel.
  - **N:** Neve 1057 input channel, germanium transistors.
  - **T:** Thermionic Culture Vulture, triode setting; warm, even-harmonic.
  - **P:** Culture Vulture, pentode setting; described as **odd-harmonic**.
- **Discrepancy to remember:** Rubbish's measurements of P (Punish off, Drive 7–10) show it strongly *lopsided* (H2 only 4–6 dB under the note), with odd harmonics dominating only briefly (Drive 3–4.5) and at max Drive + Punish. Trust the measurements over the manual's one-word description.
- **Punish [Documented]:** +20 dB of extra gain.
- **Auto [Documented]:** lowers the output as Drive rises; the Output knob moves the opposite way to Drive. Measured 2026-11-25: it doesn't hold level exactly (−3 to −10 dB depending on style).
- **Tone shaping [Documented]:**
  - **Before** the saturation: Low Cut, Thump (a few dB of boost at the Low Cut frequency, like a tape head bump) and Tone (a gentle tilt).
  - **After** the saturation: High Cut, removing fizz; Steep switches its slope from 6 to 30 dB/oct.
- **Mix [Documented]:** blends dry back, partly to restore clipped transients.
- **Modelling claim [Documented]:** the styles match how the gear *responds to dynamics* when pushed past its limits.
- **Oversampling and aliasing:** **not mentioned** by Soundtoys. Users (KVR, low reliability) report aliasing at 44.1 kHz that improves at 96 kHz, and Rubbish measured folded harmonics on a 2800 Hz note at Drive 10.
- **How Soundtoys models [Third-party: Ken Bogdanowicz in Sound On Sound, Aug 2010; Tape Op]:**
  - Linear devices can be captured by measurement; nonlinear ones need the circuit understood.
  - They listen first, then measure with tones and real material at many levels and settings.
  - The limits are CPU and incomplete understanding of the devices.

## Other modelling philosophies (Sound On Sound, Aug 2010)

- **UAD (Dave Berners):** physical circuit modelling, with device-specific test signals.
- **Softube (Niklas Odelholm):** component modelling. A distortion mismatch in their 1176 was traced to a small detector **bias** difference, which shows how much bias details matter over time.
- **McDSP:** black-box matching, separating real device behaviour from test-signal artifacts.
- **Cakewalk:** listens for "pollution" such as aliasing.

## FabFilter Saturn 2 (online help)

- **Styles [Documented]:** groups Tube, Tape, Amplifier, Transformer, Smudge, Breakdown, Foldback, Rectify, Destroy (about 28 styles).
- **Drive [Documented]:** auto-compensates the output level.
- **Per band [Documented]:**
  - Dynamics: right = compression up to pumping; left = gating/expansion.
  - Feedback: amount and frequency, with optional unrestricted self-oscillation.
  - Tone (bass/mid/treble/presence) acts on the processed signal.
- **HQ [Documented via search summary]:** 8x ("Good") or 32x ("Superb") oversampling; a Linear Phase option for the oversampling and crossover filters.

## Klanghelm IVGI2 / SDRR2

- **Asymmetry [Documented: IVGI2 manual via search summary]:** keeps the negative half cleaner and adds even harmonics.
- **Response [Documented: IVGI2 manual via search summary]:** sets which frequencies saturate most (e.g. LF+).
- **SDRR [Third-party reviews]:** Response and a Dynamics control for how transients are treated; SDRR2 modes TUBE / DIGI / FUZZ / DESK. One user reports that slower dynamics settings compress hard.

## Kazrog KClip 3 / KClip Zero

- **KClip 3 [Documented]:** 8 soft-clip modes, multiband, 2x–32x oversampling. The final Ceiling hard clip is **not** oversampled.
- **KClip Zero [Documented]:** 2x real-time, 16x offline.

## Sonnox Oxford Inflator (manual)

- **[Documented]** Effect 0–100; Curve changes loudness and tone. Clip 0dB limits processing to digital full scale.
- **Band Split [Documented]:** 3 bands to avoid intermodulation; unsplit keeps harmonic phase better and usually sounds nicer.

## Ableton Saturator (Cycling '74 abl.dsp docs)

- **[Documented]** Curves: Analog Clip, Soft Sine, Medium Curve, Hard Curve, Sinoid Fold, Digital Clip. The clips cut immediately, the curves soften, Sinoid Fold is for effects. Drive is input gain in dB.

## Airwindows (Chris Johnson)

- **Density [Documented]:** positive = thick tube-like saturation; negative = "antisaturation"; zero latency.
- **Oversampling [Documented, author's view]:** argues aliasing worries for un-oversampled saturation are overstated if the signal is smoothed or band-limited. Disputed on forums.

## Not researched (gaps)

Kazrog True Iron, Softube saturators, UAD oversampling statements, Tokyo Dawn, iZotope Trash. No interviews found with Bob Gilbert or Evan Bogdanowicz.

## Patterns worth copying (inferences)

- Every major design puts EQ on both sides of the clipper: pre-EQ chooses what distorts, post-EQ tames fizz.
- "Responds to dynamics", Dynamics knobs and cleaner-negative-half asymmetry all sell level- or transient-dependent behaviour. The mechanisms are undisclosed; envelope-driven bias or drive is plausible.
- Auto gain everywhere is a drive-linked offset or RMS-based, never exact. Level-match by measurement when comparing.
