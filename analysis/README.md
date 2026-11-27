# Analysis

Reproducible measurement of how Rubbish's **Grit** distortion compares with a commercial reference (Soundtoys Decapitator, style P). This code produces the figures and tables in [`docs/case-study.md`](../docs/case-study.md).

## What's here

| Path | What it is |
|---|---|
| `data/` | Decapitator P recorded in Logic Pro: a sine at about −13 dBFS with Drive automated 0 → 10 over 16 bars. Three takes: 44 Hz and 175 Hz with Punish off, and 44 Hz with Punish on. Left channel, 48 kHz, 24-bit FLAC |
| `rubbish_analysis/harmonics.py` | Finds the note's exact frequency, then measures harmonics 1–40 in 0.25 s Blackman-Harris windows, plus THD |
| `rubbish_analysis/grit_model.py` | A NumPy copy of the plugin's Grit signal chain (drive gain → 4x oversampling → peak follower → biased soft-clip curve), with every Grit version from the project's git history |
| `rubbish_analysis/figures.py` | The charts |
| `run_analysis.py` | Runs everything: measures P, renders each Grit version through the same sweep, scores each version and draws the figures |
| `output/` | Generated harmonic maps (CSV, one row per 0.1 Drive) and `fit-summary.md` |

## Run it

```bash
python3 -m venv .venv
.venv/bin/pip install -r requirements.txt
.venv/bin/python run_analysis.py
```

It takes about 20 seconds. Figures are written to `docs/images/` and tables to `output/`.

## Validation

- **Measurement:** this pipeline reproduces the P harmonic maps first measured during development (`measurements/p-harmonic-map-*.csv`) to within 0.1 dB on average (harmonics 2–6).
- **Model:** run on the Grit version that was in the plugin when those maps were measured (v2), it reproduces that version's measured map (`measurements/grit-harmonic-map-175hz.csv`) to within 0.06 dB.
