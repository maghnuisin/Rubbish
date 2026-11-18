# Tuning map: what controls each feel or sound

Use this to turn a plain-language complaint into a specific, reversible change, and to tell whether it is an **internal setting** (adjustable within the current prd.md) or a **spec value** (fixed by prd.md, so it needs a prd.md update first).

Paths are relative to `Source/`. Values are as of the version this skill was written for. Always re-read the file before proposing a change, because values may have been tuned since. When proposing, give the old value so it can be restored.

## Spec values (fixed by prd.md: change prd.md first)
Everything in prd.md's parameter tables: every knob's **range, default and unit**, and the Push amount (+20 dB, `Parameters.h: pushBoostDb`). Also anything written in its Behaviour or Interface sections, such as the 0 dBFS output ceiling, no user oversampling control, and the fixed reaction speed of the loudness tracker (it may be *tuned*, but not exposed as a knob).

Changing a range or default also affects saved Logic projects, so mention that when proposing it.

## Internal settings (tunable within the current spec)

| Complaint (owner's words, roughly) | Setting | Where | Current | Direction |
|---|---|---|---|---|
| "Tape too soft/too harsh" | Tape curve is `tanh`, no parameter | `dsp/Waveshapers.cpp: tape` | tanh | Would need a curve change; propose carefully |
| "Console not punchy/too clean" | Knee: how long it stays clean before bending | `dsp/Waveshapers.cpp: kneeEnd` | 1.5 | Lower = breaks up sooner, more aggressive |
| "Tube not thick/too buzzy" | Asymmetry (even-harmonic "thickness") | `dsp/Waveshapers.cpp: bias` | 0.35 | Higher = thicker/buzzier; lower = cleaner |
| "Synth not raw enough/too harsh" | Knee hardness | `dsp/Waveshapers.cpp: hardness` | 8 | Higher = harder, more biting; lower = softer |
| "Click when changing Character" | Character crossfade time | `dsp/CharacterShaper.h: crossfadeSeconds` | 0.03 s | Longer = smoother switch |
| "Drive moves feel steppy/laggy" | Drive smoothing | `dsp/DistortionStage.h: driveRampSeconds` | 0.05 s | Shorter = snappier, longer = smoother |
| "Peak doesn't howl enough/too wild" | Resonance at 100% Peak | `dsp/ResonantFilter.cpp: maximumQ` | 40 | Higher = sharper howl |
| "Resonance gets too loud/screams too much" | How hard the resonance is reined in | `dsp/ResonantFilter.cpp: stateLimit` | 3.0 | Lower = tamer, grittier resonance |
| "Filter sweeps feel laggy" | Cutoff/Peak smoothing | `dsp/ResonantFilter.h: smoothingSeconds` | 0.03 s | Shorter = more immediate |
| "Filter Order switch gap/click" | Fade length when swapping | `dsp/FilterOrderSwitch.h: fadeSeconds` | 0.01 s | Longer = smoother but longer dip |
| "Filter Follow barely moves/too much" | Maximum movement at 100% | `dsp/FilterFollow.h: maxOctaves` | 4 octaves | Higher = bigger sweeps |
| "Filter Follow ignores quiet playing" | Quietest level that still moves it | `dsp/FilterFollow.h: quietestDb` | −48 dB | Lower = reacts to quieter sounds |
| "Follow/Restore react too slowly/jumpily" | Shared loudness tracker speed (prd: fixed, tuned, not a knob) | `dsp/LoudnessTracker.h: attackMs / releaseMs` | 1 ms / 80 ms | Longer release = smoother, slower |
| "Dynamics Restore too subtle/too snappy" | Average it compares against | `dsp/DynamicsRestore.h: averageSeconds` | 0.3 s | Longer = restores longer-term shape |
| "Dynamics Restore over-boosts hits" | Max boost/cut | `dsp/DynamicsRestore.h: maxBoostDb / maxCutDb` | +18 / −24 dB | Smaller = gentler |
| "Spread not wide enough/too wide" | Extra drive on the sides | `dsp/StereoSpread.h: sideDriveBoost` | 4× (+12 dB) | Higher = wider, grittier sides |
| "Clean Low crossover clicks when moved" | Crossover smoothing | `dsp/CleanLowSplitter.h: smoothingSeconds` | 0.05 s | Longer = smoother |
| "Auto Gain swells/pumps after changes" | Loudness averaging window | `dsp/LoudnessMatcher.h: averagingSeconds` | 0.4 s | Longer = steadier but slower to settle |
| "Auto Gain jumps too much" | Range it may use | `dsp/OutputSection.h: autoGainMatcher {…, −40, +12}` | −40…+12 dB | Narrower = more conservative |
| "Delta too loud/too quiet" | Max lift for Delta | `dsp/OutputSection.h: deltaMatcher {…, −24, +24}` | ±24 dB | Adjust upper value |
| "Limiter pumps/ducks" | Limiter recovery | `dsp/SafetyLimiter.h: releaseSeconds` | 0.1 s | Longer = smoother, less pumping |
| "Mix/Output changes feel steppy" | Output-section smoothing | `dsp/OutputSection.h: rampSeconds` | 0.05 s | — |
| "Meter falls too fast/slow" | Meter fall speed | `ui/LevelMeter.h: fallDbPerRefresh` | 1.5 dB per frame (~45 dB/s) | Smaller = slower fall |
| "Can't read the peak number in time" | Peak-number hold | `ui/LevelMeter.h: readoutHoldRefreshes` | 45 frames (1.5 s) | More = longer hold |
| "Fine drag too slow/fast" | Fine-adjust ratio | `ui/FineDragSlider.h: fineScale` | 0.1 (10× slower) | — |
| "Knobs too sensitive/sluggish" | Pixels of drag for a full knob turn | JUCE default; not set yet. It would be added with `knob.setMouseDragSensitivity (…)` in `ui/LabelledKnob.cpp` | 250 px | More = slower |
| "Window too small/big at its limits" | Resize range | `PluginEditor.cpp: minimumScale / maximumScale` | 0.9 … 1.8 | — |
| "Text too small" | Base design size / font sizes | `ui/MainLayout.h: designWidth/Height`, label fonts in `ui/` | 1020 × 560 | Usually raise fonts rather than size |

## Not tunable by ear (leave alone unless there is a measured problem)
- Oversampling factor (`dsp/DistortionStage.h: oversamplingStages`, 4×). This affects fizz and latency. prd.md forbids a user control, but the internal amount could rise if fizz is heard. That costs CPU and latency.
- DC blocker (`dsp/DistortionStage.h: dcBlockerHz`, 10 Hz) and limiter ceiling (`dsp/SafetyLimiter.cpp: ceiling`, −0.3 dBFS). These are safety values.
