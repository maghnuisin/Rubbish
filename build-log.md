# Rubbish — Build Log

One entry per session, newest at the top. Each entry: what changed, what was decided, what's open, what's next.
Read this at the start of every session. Add an entry at the end of every session.
(A copy is also kept in the claude.ai "Rubbish" project as docs/build-log.md; this file in the repo is the main one.)

---

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

**Next**
- Find what P has that Grit lacks: compare full spectra (harmonics well above H5, non-harmonic noise/crackle, any movement over time) using the Desktop recordings and `measurements/`.
- (Was:) Owner listening test: new Grit vs Decapitator P, Push off then on, especially Drive 7–10 (Rubbish 25–36 dB). Quit and reopen Logic first.
- If Grit is approved, measure and refit Tape (A), Tube (T), Heavy (E) and Edge (N) the same way (gentler and warmer at low Drive, dirtier and more lopsided at high Drive), at the real Logic level. Then measure the other Characters the same way.
- Then continue version 3 stage 2 (the LFO section UI).
