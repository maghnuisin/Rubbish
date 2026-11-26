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

**Next**
- Listening test of each Character with Push off and on, on bass, drums and vocals. Compare Heavy and Edge with Push against the old Console and Synth sound.
- Then continue version 3 stage 2 (the LFO section UI).
