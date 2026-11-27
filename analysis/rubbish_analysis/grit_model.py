"""A NumPy copy of Rubbish's Grit Character, for rendering sweeps offline.

Mirrors Source/dsp/Waveshapers.cpp, CharacterVoicing.cpp and CharacterShaper.cpp:
drive gain -> 4x oversampling -> peak follower -> biased soft-clip curve -> back to 1x.
Every Grit version from the project's history is kept so the fits can be compared.
"""

from dataclasses import dataclass

import numpy as np
from scipy.signal import resample_poly

MAX_DRIVE_DB = 36.0
PUSH_DB = 20.0
OVERSAMPLING = 4
PEAK_RELEASE_SECONDS = 0.1
UNLIMITED = 1.0e6


@dataclass(frozen=True)
class Voicing:
    sensitivity_db: float
    bias_at_zero: float
    bias_across_drive: float
    bias_from_push: float
    hardness: float
    hardness_from_push: float
    bias_follows_gain: float
    bias_limit_ratio: float
    bias_limit_across_drive: float = 0.0
    hardness_across_drive: float = 0.0


# Grit's history (git log of Source/dsp/CharacterVoicing.cpp).
GRIT_VERSIONS = {
    "v0 fitted to THD at -22 dBFS": Voicing(14.092, 0.133, -0.405, -0.152, 2.484, 0.000, 0.701, 0.900),
    "v1 refit at real Logic level": Voicing(2.198, 0.319, -0.824, -0.417, 2.166, -1.021, 0.695, 0.950),
    "v2 refit incl. upper harmonics": Voicing(0.000, -0.841, 2.275, 0.832, 1.604, 0.280, 0.497, 0.609),
    "v3 retuned to harmonic maps": Voicing(-0.971, -0.802, 1.832, 1.531, 1.726, 0.279, 0.479, 0.210, 0.702, -0.402),
}


def db_to_gain(db):
    return 10.0 ** (np.asarray(db) / 20.0)


def soft_clip(x, hardness):
    return x / (1.0 + np.abs(x) ** hardness) ** (1.0 / hardness)


def peak_follower(x, sample_rate):
    """Instant attack, exponential release: peak[n] = max(|x[n]|, r * peak[n-1]).

    Solved without a Python loop: in the log domain the recursion is a running maximum.
    """
    log_release = -1.0 / (PEAK_RELEASE_SECONDS * sample_rate)
    n = np.arange(len(x), dtype=np.float64)
    log_abs = np.log(np.abs(x) + 1e-30)
    return np.exp(n * log_release + np.maximum.accumulate(log_abs - n * log_release))


def curve_settings(voicing, drive_db, push):
    """Per-sample curve settings, as computed by computeCurveSettings()."""
    v = voicing
    drive_fraction = np.clip(drive_db / MAX_DRIVE_DB, 0.0, 1.0)
    total_db = drive_db + PUSH_DB * push
    base_bias = v.bias_at_zero + v.bias_across_drive * drive_fraction + v.bias_from_push * push
    bias = base_bias * db_to_gain(total_db + v.sensitivity_db) ** v.bias_follows_gain
    hardness = v.hardness + v.hardness_across_drive * drive_fraction + v.hardness_from_push * push
    limit_ratio = v.bias_limit_ratio + v.bias_limit_across_drive * drive_fraction
    return db_to_gain(v.sensitivity_db), bias, hardness, limit_ratio


def shape(x, input_gain, bias, hardness, limit_ratio, peak):
    """Waveshapers::shape(): bias capped below the signal's own peak, centre removed, normalised."""
    limit = limit_ratio * input_gain * peak
    bias = np.clip(bias, -limit, limit)
    centre = soft_clip(bias, hardness)
    return (soft_clip(input_gain * x + bias, hardness) - centre) / (input_gain * (1.0 + np.abs(centre)))


def render(signal, sample_rate, drive_db, voicing, push=0.0):
    """Runs a signal through Grit with a per-sample Drive curve (dB, 0..36)."""
    total_db = drive_db + PUSH_DB * push
    driven = resample_poly(signal * db_to_gain(total_db), OVERSAMPLING, 1)
    drive_up = np.repeat(drive_db, OVERSAMPLING)[:len(driven)]
    peak = peak_follower(driven, sample_rate * OVERSAMPLING)
    shaped = shape(driven, *curve_settings(voicing, drive_up, push), peak)
    return resample_poly(shaped, 1, OVERSAMPLING)
