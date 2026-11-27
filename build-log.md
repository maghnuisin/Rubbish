# Rubbish — Build Log

One entry per session, newest at the top. Each entry: what changed, what was decided, what's open, what's next.
Read this at the start of every session. Add an entry at the end of every session.
(A copy is also kept in the claude.ai "Rubbish" project as docs/build-log.md; this file in the repo is the main one.)

---

## 2026-11-27 — Hunting P's "fizz and phaser"

**What changed**
- No code changes. Owner described what P has that Grit lacks: a high fizz plus a pitch-wobble/phaser feel that comes forward in the last quarter of the Drive range (and Decapitator's Output drops automatically as Drive rises; Rubbish's Auto Gain already does that internally).
- Found two candidates in the recordings (details in `decapitator-analysis.md`, testing note 8):
  1. P **aliases** at high Drive: harmonics fold back as out-of-tune high tones, which move against the pitch and can sound phaser-like. Rubbish deliberately prevents this (prd.md: no audible aliasing).
  2. While Drive is **moving**, P adds a 7–10 Hz flutter around every harmonic, which stops when the knob is still.
- The owner moved the recordings into the Desktop folder "rubbish Test files".

**Open questions**
- Which of the two the owner hears: test by holding P's Drive still at 8–9.
- If it's aliasing: matching it conflicts with prd.md, so it would need the owner's decision (e.g. an option, or accept the difference).

- Owner test result: with P's Drive held still the high fizz stays (fits aliasing); the phase effect only happens while turning the knob and sounds like "a narrow accentuated notch sweeping up the spectrum". Not yet explained.
- Owner asked for a research skill on distortion DSP (academic papers and engineering practice), built in a fresh session.
- **Research skill built** (no code changes, not committed): `.claude/skills/distortion-dsp/`. It holds cited notes on curve shape vs harmonics, tube/tape memory effects, aliasing and oversampling, smoothing, circuit modelling, how other saturators are documented, a measurement cookbook, and a source list. Future sessions load it automatically when working on Characters, Drive, aliasing or matching Decapitator.
  - Best explanation for P's sweeping notch: curves with a sharp bend or a lopsided bias make particular harmonics cancel out at particular Drive settings, and that gap climbs the harmonics as Drive rises, so it is only noticeable while the knob moves. The 7–10 Hz flutter looks like P updating Drive in steps (copying that would break prd.md's "never zips" rule).
  - Tests for each cause are in `.claude/skills/distortion-dsp/references/rubbish-grit-vs-p.md`.
- **Harmonics-vs-Drive map done** (no code changes): P's sweeping notch is confirmed as gaps in its harmonics that climb as Drive rises (on a 175 Hz note, through about 3–5 kHz between Drive 4.7 and 6). Grit's gaps stop moving from about Drive 6, because its lopsidedness is capped in step with the signal, so its tone freezes. Data and pictures are in `measurements/harmonic-map-*`; details are in `decapitator-analysis.md`.
  - Open question for the owner: retune how Grit's lopsidedness follows Drive (it touches the safeguard that stops quiet notes going silent, and Tube/Heavy use the same mechanism).
- **Tried the retune offline (owner said yes):** a computer search found Grit numbers that match P's main harmonics better and make its gaps move between Drive 3.5 and 7. But above about Drive 8, where the owner hears P's effect, Grit's tone still freezes, whatever the numbers. Likely reason: P's wave keeps getting sharper corners, and Grit's rounded curve can't. Nothing was changed in the plugin. Details are in `decapitator-analysis.md`.
  - Owner chose to try a new design. Tried a hard ceiling (sharp corners) after Grit's curve, offline. It is no clear improvement on paper either. Made loudness-matched listening files of the same bass swept through P and three Grit versions: Desktop → rubbish Test files → `Grit candidates (bass F2, Drive 0 to 10)`. Owner picked **3 Grit retuned** by ear.
- **Grit retuned in the plugin** (`Source/dsp/CharacterVoicing.cpp`): new numbers, plus two new per-Character settings (how the lopsidedness limit and the knee change across Drive). They are zero for Tape, Tube, Heavy and Edge, so those sound exactly as before. Checked offline: never silent (better than before), no extra aliasing. Built, installed, no warnings, **auval passed**. Not committed.
  - To hear it: quit and reopen Logic, play the bass through Grit and sweep Drive, especially the top half (Rubbish Drive 18–36 dB).

**Next**
- ~~Run the "harmonics vs Drive" map~~ Done the same day (below).
- Then: decide on aliasing-style fizz vs prd.md, and investigate P's sweeping notch while Drive moves.

## 2026-11-26 — New Characters built into the code

**What changed**
- Version 3 (Twist + LFO section) is mid-build. The half-finished LFO work was saved as a git checkpoint first (commit `4759f7e`).
- Code (commit `66222b3`):
  - Characters are now **Tape, Tube, Heavy, Edge, Grit** (modelled on Decapitator A, T, E, N, P). Console and Synth are gone. Character is still a dropdown.
  - Each Character's curve was fitted to the Decapitator measurements and changes as Drive turns, not just in level.
  - **Push** adds +20 dB *and* a harder curve.
  - Old projects open on the nearest new Character: Console → Edge, Synth → Heavy.
  - Changing Drive, Push or Character glides or crossfades, so nothing clicks.
- Offline checks:
  - Each Character's sine sweep follows its Decapitator curve.
  - No Character goes silent at any input level or Drive (Tube, Heavy and Grit cap their bias below the signal's own peak).
  - No NaNs, and no clicks when switching mid-note.
- `prd.md`, `testing-notes.md` and `decapitator-analysis.md` updated; the analysis has a "Rubbish fit" table to check future changes against.
- **Built and installed:** no code warnings; AU and VST3 installed; **auval passed**.
- Added `Build Rubbish.command`: double-click to build, install and run auval (output goes to `build/build-output.txt`).

**Decisions**
- Owner confirmed removing Console and Synth, even though the 18 Nov notes called them the favourite "full and fat" sound with Push.

**How building works**
- Claude Code on the Mac (in this folder) builds and installs directly, so the owner just reopens Logic.
- Cloud sessions can't build Mac plugins; they open `Build Rubbish.command` instead.

**Open questions**
- Known gaps vs Decapitator, to judge by ear: Tube and Grit fold over only gently at the top with Push; Heavy + Push tops out lower; no mode is as dark on top as Decapitator.
- With Auto Gain off, hard-driven Grit (and to a lesser extent Edge) comes out quieter.
- In old projects, DAW automation lanes for Character may land on different modes (the list went from 4 to 5 entries).

**Later the same day: listening test, Grit vs Decapitator P**
- Owner: all Characters lack grit near full Drive compared with the real Decapitator. Focused on Grit vs P with Push off.
- Owner's "no knobs turned" recordings looked very different. Found why in Logic: the sine reaches the plugins at about −13 dBFS (the recordings were post-fader, so they looked 8 dB quieter). At that level Grit at Drive 0 measures exactly what the owner recorded (18%), vs P's 6%.
- Claude recorded P at Drive 0–10 in Logic (Desktop: `P test drive 0…10.wav`) and compared Grit at the same level. Table in `decapitator-analysis.md`. Grit is too dirty and buzzy at low Drive, and at high Drive it levels off and stays even-sided while P keeps getting dirtier and very lopsided (huge 2nd harmonic).
- Root cause: the Characters were fitted at about −22 dBFS and to total distortion only, not to the harmonic mix or to louder inputs.
- At the owner's request, also swept P's Drive continuously (automation 0 → 10 in Logic, bounced) and Grit the same way offline; data every 0.1 Drive in `measurements/`. P has a hollow "turnover" around Drive 3–4.5, then shoots up and turns very lopsided; Grit is smooth, hollow through the middle and flat after 7.
- Also swept P with Punish on vs Grit with Push on (data in `measurements/`). P + Punish peaks ~73% at Drive 7 then folds back to ~48% and changes colour; Grit + Push is stuck at 41–48% whatever the Drive, so Drive is nearly dead with Push on.
- Logic project left as it was (automation undone, Decapitator bypassed with Drive 10 / Punish on, Rubbish on, Tape, Event List closed). Not saved.

- **Grit refitted** (`Source/dsp/CharacterVoicing.cpp`, numbers only, same curve design) by a computer search against the P sweeps. Now follows P closely with Push off across 0–10 and with Push up to about Drive 8.5; with Push at 9–10 it stays ~70% where P eases to ~50%. Never silent from −40 to 0 dBFS. Built, installed, no code warnings, **auval passed**.
- Open question for the owner: prd.md says Push adds "more odd harmonics" and Grit "folds over" at the top; the measured P + Punish is actually more even/lopsided, and the new Grit follows that, with only a weak fold-over.

- Owner listened to the refitted Grit: "Still not as gritty, dirty. There is more harmonics in the P version that we are not matching. There is something happening that we are missing."

- Full-spectrum check of P vs new Grit (details in `decapitator-analysis.md`): on a steady low sine they're close; the real gap is P's brighter upper-harmonic tail at Drive 4.5–7.5 (15–20 dB more above ~750 Hz). Sweep "noise" is Decapitator reacting to automation, not its steady sound. Loudness isn't the cause. Untested: higher notes / real bass (possible tone shaping before the clipper).

- Owner chose to record more P. Claude recorded P (Punish off) in Logic with Drive automated 0→10 on 175 / 700 / 2800 Hz notes and on a Massive bass preset (Analog Classique 1, F2), plus the dry bass. Files on the Desktop; Logic restored and not saved.
  - P treats every note the same (no hidden tone shaping). On the bass the gap was again P's brighter top end at Drive 3–8.
  - Cause: P's curve has a definite bend point, so it makes high harmonics even at moderate Drive; Grit's curve is perfectly smooth.
- **Grit refitted again** (numbers only, same curve design), now also matching P's upper harmonics. From Drive 4.5 up the top end follows P, Push off and on; Push now eases off a little at the top like Punish. Never silent; built, installed, no warnings, **auval passed**.
  - Side effect: a faint extra buzz at Drive 0–2.5, because Grit's lopsidedness limit follows the level with a short release and wobbles on low notes. A slower follower (about 2 s) fixes most of it in the model, but that changes how the plugin works (Tube and Heavy use it too), so it needs the owner's OK.

- Owner listened: "the distortion is unchanged to my ears… no closer to P". Claude made loudness-matched files of the same bass through P, old Grit and new Grit (Desktop: "Grit vs P listening test"). Measured on those files, P and new Grit are close in harmonics, top end, how raspy each cycle is, and lopsidedness, so these steady-sweep measurements can't see what the owner hears.

**Next**
- Owner: say whether 1 P vs 3 new Grit in the listening-test files sound clearly different (then something in the distortion itself is still missing), or whether the difference only shows in Logic (then check settings: Punish on, Drive match, loudness, Twist).
- Slower level follower for the low-Drive buzz: on hold until the above is settled.
- (Done:) record P with higher notes and a real bass.
- (Earlier:) Find what P has that Grit lacks: compare full spectra (harmonics well above H5, non-harmonic noise/crackle, any movement over time) using the Desktop recordings and `measurements/`.
- (Was:) Owner listening test: new Grit vs Decapitator P, Push off then on, especially Drive 7–10 (Rubbish 25–36 dB). Quit and reopen Logic first.
- If Grit is approved, measure and refit Tape (A), Tube (T), Heavy (E) and Edge (N) the same way (gentler and warmer at low Drive, dirtier and more lopsided at high Drive), at the real Logic level. Then measure the other Characters the same way.
- Then continue version 3 stage 2 (the LFO section UI).
