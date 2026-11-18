# Test recipes for Rubbish

Short experiments to suggest during a listening session. Pick 3–5, adapted to the owner's music. Each lists **setup**, **what to do**, and **what to listen or watch for**. All assume Rubbish is on an audio track in Logic. The defaults are Drive 6 dB, Tape, Auto Gain on, Mix 100%, everything else at zero or off.

General Logic tips to pass on when relevant:
- **Bypass to compare:** the power button on the plugin slot in Logic's mixer turns Rubbish off and on. Auto Gain keeps the levels close, so the comparison is fair.
- **Mono check:** put Logic's **Gain** plugin *after* Rubbish and switch on its **Mono** button.
- **Loop a bar:** press C for cycle mode and drag across the bar ruler, so the same bit repeats while they twist knobs.
- **Safety:** turn speakers down before high Peak, Push or Scream.

## Contents
1. Drive and Character
2. Filters
3. Clean Low, Dynamics Restore, Spread
4. Output section, Delta and meters
5. Feel and layout
6. prd.md acceptance tests (blind listening)

---

## 1. Drive and Character

**Character tour.** Drum loop, Drive ~18 dB. Click Tape → Console → Tube → Synth while it loops.
Listen for: do the four feel clearly different? Tape = warm and rounded; Console = punchy and forward; Tube = thick and buzzy; Synth = raw and biting. Any switch that clicks?

**Drive sweep for fizz (aliasing).** Single sustained synth note or vocal, Synth character. Slowly turn Drive from 0 to 36, then press Push.
Listen for: grit that stays musical versus a thin, glassy, "digital" hiss that doesn't follow the pitch. That hiss would be aliasing, and prd.md says there should be none. High-pitched sources show it most.

**Push jump.** Bass, Drive 12. Toggle Push on and off.
Listen for: a big step in dirt (+20 dB of drive), without a pop at the moment of switching.

**Dead zones.** Any source. Drag Drive slowly across its whole range.
Listen for: stretches of knob travel where nothing changes, or where everything happens in a tiny slice. Note *where on the knob* it felt off.

## 2. Filters

**Classic sweep.** Drum loop or synth, Drive ~12, High Cut ~1 kHz, High Peak 60–70%. Slowly sweep High Cut up and down.
Listen for: a vocal "wah" resonance; the sweep should be smooth with no stepping or zipper noise.

**Howl safety.** Low Peak and High Peak at 100%, Push on, then click the ↔ swap to Scream.
Listen and watch: it may scream, but must never blow up, crackle uncontrollably or hit the top of the Out meter in orange for long. After stopping playback, any ringing should fade away on its own.

**Bass bump.** Bass, Low Cut ~60–80 Hz, Low Peak ~25%.
Listen for: a tighter low end with a slight bump, versus thin or boomy.

**Filter Order feel.** Same settings, click ↔ back and forth.
Listen for: Clean first (smoother, filters tame the grit) versus Scream (filters drive into distortion, more aggressive). Is the difference worth a button?

**Filter Follow.** Drums, High Cut ~800 Hz, Filter Follow +70%, then −70%.
Listen for: hits making the sound brighter (+) or darker (−). Does it react naturally, too slowly or too jumpily? Is ±100% too much or not enough?

## 3. Clean Low, Dynamics Restore, Spread

**Clean Low on bass.** Bass or 808, Drive 24+. Turn Clean Low from 20 Hz up to ~150 Hz.
Listen for: the low end becoming solid and clean while the upper grit stays. Any phase-y hollowness around the crossover?

**Dynamics Restore on drums.** Drum loop, Drive 30 + Push (squashed). Sweep Dynamics Restore 0 → 100%.
Listen for: the hits' punch returning. Does 100% sound natural or over-snappy? Is anything happening below 30%?

**Spread width.** A *stereo* source (stereo synth pad, room mics, stereo drum bus), Drive ~18. Sweep Spread 0 → 100%, then do the mono check.
Listen for: wider sound in stereo; in mono, no level drop or hollow sound. A mono source (single mic) should *not* change with Spread. That's expected, not a bug.

## 4. Output section, Delta and meters

**Auto Gain fairness.** Drums. Turn Drive 0 → 36 with Auto Gain on, then off.
Listen for: with it on, the level stays roughly steady, though it settles over about a second after big changes. Does that settling feel like an annoying "swell"?

**Parallel blend.** Drums, heavy settings, Mix ~30–50%.
Listen for: original punch plus added grit, and no flamming or phasey doubling (that would mean timing misalignment, a bug).

**Delta.** Any source, press Delta, move Drive and Character.
Listen for: only the added grit, at a clearly audible level. Mix and Auto Gain shouldn't change it.

**Meters.** Watch In and Out while playing.
Watch for: numbers easy to read; the peak number holds long enough to read (~1.5 s); the bars move smoothly; Out never above 0 dB.

## 5. Feel and layout

- **Knob feel:** drag each knob; hold Shift for fine control; double-click to reset; scroll to nudge; click a value and type one. Anything awkward or surprising?
- **Labels and wording:** is every name understandable without explanation? Which names would they change?
- **Layout:** is the flow In → Drive ↔ Filters → Output obvious? Is the ↔ button discoverable?
- **Resizing:** drag the window corner smaller and larger. Is it readable at the smallest size on their screen?
- **Recall:** set something unusual, save the Logic project, quit, reopen. Everything should come back exactly.

## 6. prd.md acceptance tests (blind, volume-matched)

prd.md's sound-quality acceptance: *in blind, volume-matched listening tests on drums, bass, vocals and synth, each Character is judged musically usable, with no audible aliasing at any Drive setting.*

How to run it:
1. Four tracks (drums, bass, vocal, synth), each with Rubbish, Auto Gain **on** (this is what volume-matches it).
2. **Blind:** ask a friend to pick the Character and Drive for each track without telling them, or have them toggle bypass while they listen and guess. With nobody to help, at least close your eyes while the settings get changed.
3. For each Character on each source, score: *usable as-is* / *usable with care* / *not usable*, plus any fizz heard (yes/no, at what Drive).
4. Record results in testing-notes.md as one session. "Not usable" or "fizz heard" items count as **bugs or tuning** against the spec.
