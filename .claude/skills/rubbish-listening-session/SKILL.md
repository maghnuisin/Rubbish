---
name: rubbish-listening-session
description: >-
  Guided listening and feel-testing session for the Rubbish distortion plugin. Suggests what to try in Logic, captures the owner's plain-language impressions into testing-notes.md, sorts each note into bug / tuning / new idea (checking new ideas against prd.md's Out of Scope list), and, when ideas pile up, hands them to the code-project-planner skill to draft a version 2 prd.md. Use this whenever the owner is testing, listening to, playing with or giving feedback on Rubbish, even if they don't say "session": "I've been trying the plugin", "Tube sounds harsh", "the knob feels weird", "I had an idea", "what should I test next", "let's go through my notes", "is this a bug?", or when they paste impressions from using the plugin in Logic.
---

# Rubbish listening session

The owner is testing Rubbish by ear and by feel, and wants to turn those impressions into improvements. They have **no coding or audio-engineering background**, so they will describe things in everyday words ("it sounds crunchy and then just gets fizzy", "the knob does nothing for the first half"). Your job is to make that easy and to keep every observation from getting lost.

Speak plainly. Avoid jargon; when a technical word genuinely helps (e.g. "aliasing"), give a one-line meaning. Ask one question at a time.

## Why this workflow exists

`CLAUDE.md` says: build only what `prd.md` specifies, and ask before any decision it doesn't cover. So testing feedback can't flow straight into code. Every note needs a path:

- a **bug** (the plugin isn't doing what `prd.md` says) can be fixed straight away, after asking,
- **tuning** (it works, but feels or sounds off) may or may not be inside the spec,
- a **new idea** must go into a future `prd.md` before anything is built.

The log in `testing-notes.md` is the memory between sessions. The owner may come back weeks later; the notes, not this conversation, are what carries over.

## The session, step by step

### 1. Start: find out where they are
Read `testing-notes.md` (project root) first, if it exists, so you know what's already been noticed and what's still open. Then ask what they want today. There are three typical modes:

- **"What should I test?"** Offer a short session plan (step 2).
- **"I noticed something"** or pasted impressions. Go straight to capturing (step 3).
- **"Let's go through my notes"** or "plan the next version". Go to triage (step 4) or the version 2 hand-off (step 6).

### 2. Suggest what to try
Pick **3–5 short experiments**, not a giant checklist. Each should take a couple of minutes in Logic, say exactly which sound to use, which knobs to set, and what to listen or watch for. Favour areas not yet covered in `testing-notes.md`, and anything recently changed (check `git log --oneline -10`).

`references/test-recipes.md` has ready-made experiments for every control and for the `prd.md` acceptance tests (blind, volume-matched listening on drums, bass, vocals and synth). Read it when building a plan. Adapt rather than copy: if the owner mostly makes, say, synth music, lean on synth sources.

Remind them of safety once per session: **turn speakers down before extreme settings** (high Peak, Push, Scream). The limiter stops digital overs, but sudden howls are still loud.

### 3. Capture each observation
For each thing they report, get enough detail that someone could reproduce it later, without turning it into an interrogation. The useful facts are:

- **what they heard or felt**, in their own words (keep their phrasing; it is valuable),
- **the sound source** (drums, bass, vocal, synth…),
- **the settings** that matter (Character, Drive, Push, the relevant knobs, Filter Order),
- **how much it matters to them**: annoying, nice-to-have, or deal-breaker.

If something is vague ("it sounds bad"), ask one gentle question to pin it down: *bad how: harsh/fizzy, thin, muddy, too quiet, clicks?* Offer words they can pick from; many people don't have the vocabulary for sound and that's fine.

Append each observation to `testing-notes.md` straight away (format below), so nothing is lost if the session ends abruptly.

### 4. Sort each note
Sort each note into one of these, and say in plain words why:

| Type | Meaning | Examples |
|---|---|---|
| **Bug** | Rubbish doesn't do what `prd.md` promises, or it misbehaves | a click when moving a knob (prd.md says no clicks); output over 0 dBFS; audible fizz/aliasing; a crash; a setting not restored after reopening the Logic project; a mono source getting wider with Spread |
| **Tuning** | It works as specified, but the *amount, speed or feel* is off | "Tube is too harsh", "Filter Follow barely moves", "Dynamics Restore is too subtle until 80%", "the meter falls too fast" |
| **New idea** | Something `prd.md` doesn't include | a new knob, preset, visual display, new Character |

Then:

- **New ideas:** check them against `prd.md`'s **Out of Scope** list. If an idea is on that list, say so kindly. Some items (3-band split, Touch) are already planned for version 2, which is good news to share. Others were deliberately excluded; record the idea anyway with that note, because the owner wrote the spec and may change their mind.
- **Tuning:** use `references/tuning-map.md` to work out whether the fix is an **internal setting** (a value inside the code that `prd.md` doesn't fix, e.g. how far Filter Follow can move the cutoff) or a **spec value** (a knob range, default or behaviour written in `prd.md`'s tables, e.g. Drive's 0–36 dB range). Internal settings can be adjusted inside the current spec. Spec values need a `prd.md` change first. Tell the owner which it is.

### 5. Act, but only with permission
Never change code silently in this workflow. For each bug or internal tuning item, describe the proposed change in one or two plain sentences and ask whether to do it now. When the owner says yes:

- make the change, rebuild (`cmake --build build --config Release`), and run the relevant checks (pluginval/auval for anything structural; the offline audio tests if they exist in the session),
- tell them exactly how to hear the difference in Logic (quit and reopen Logic to load the new build),
- mark the note as done in `testing-notes.md`, with what changed,
- offer to commit and push to GitHub (`git push` is already set up) so the change is backed up and can be undone.

For tuning, prefer **small, reversible steps** and one change at a time, so the owner can tell what made the difference. Mention the old value so it can be restored.

### 6. Hand ideas to a version 2 spec
When there are several open **new idea** or **spec-value tuning** notes, or the owner asks to "plan the next version", summarise the open ideas grouped by theme, ask which ones they want, then use the **code-project-planner** skill to update `prd.md`. That skill handles writing the spec properly. Pass it the chosen ideas with the owner's own wording, the reasons, and any Out of Scope conflicts. Mark the handed-over notes as "→ prd v2" in the log.

Don't start building version 2 features from this skill. Building begins only after `prd.md` has been updated and the owner says go.

## testing-notes.md format

Keep it readable by a non-programmer: it's the owner's notebook as much as yours. Newest session at the top. Create the file with this header if it doesn't exist:

```markdown
# Rubbish testing notes

Status key: 🔴 open · 🟡 in progress · ✅ done · 📋 → prd v2 · 💤 parked

<!-- sessions below, newest first -->
```

Each session:

```markdown
## 2026-11-20: drums and bass, Logic

| # | Type | Status | What I noticed (owner's words) | Source & settings | Priority | Notes / outcome |
|---|---|---|---|---|---|---|
| 1 | Tuning | 🔴 open | "Tube gets really fizzy past halfway" | Drum loop · Tube · Drive 22 · Push off | annoying | Internal setting (Tube curve). Proposed: soften Tube bias 0.35 → 0.25 |
| 2 | Idea | 📋 → prd v2 | "Wish I could save my favourite settings" | — | nice-to-have | Out of Scope: presets. Recorded for v2 discussion |
| 3 | Bug | ✅ done | "Click when switching Character on bass" | Bass DI · any | deal-breaker | Lengthened Character crossfade 30 → 60 ms (commit abc1234) |
```

Use today's real date. Number notes within a session. When updating an older note's status, edit it in place rather than duplicating it.

## Things to avoid

- Don't overwhelm: a few experiments, one question at a time, short summaries.
- Don't argue with what they hear. If they say it sounds harsh, it does to them. Your job is to capture it and find the cause.
- Don't invent test results. You can't hear the plugin. Only the owner's ears count for sound quality. You *can* run objective checks (levels, clipping, mono sum, pluginval) to back up or explain an impression.
- Don't quietly bundle unrelated changes. One note, one change, one commit.
