"""Measure a distortion's harmonics from a recording of a sine swept through it."""

import numpy as np
from scipy.signal import get_window

NUM_HARMONICS = 40
WINDOW_SECONDS = 0.25
FFT_SIZE = 1 << 17


def estimate_fundamental(signal, sample_rate, start_s=1.0, length_s=2.0):
    """Finds the note's frequency to a fraction of a hertz (parabolic peak interpolation)."""
    start = int(start_s * sample_rate)
    segment = signal[start:start + int(length_s * sample_rate)]
    spectrum = np.abs(np.fft.rfft(segment * get_window("blackmanharris", len(segment)), FFT_SIZE * 4))
    peak = int(np.argmax(spectrum[1:])) + 1
    a, b, c = np.log(spectrum[peak - 1:peak + 2] + 1e-30)
    offset = 0.5 * (a - c) / (a - 2 * b + c)
    return (peak + offset) * sample_rate / (FFT_SIZE * 4)


def harmonic_levels_db(segment, sample_rate, fundamental_hz, num_harmonics=NUM_HARMONICS):
    """Level of harmonics 1..n in dB relative to the fundamental.

    Each harmonic is the spectral peak within 30% of a harmonic spacing of k * f0,
    so slight pitch drift in a recording doesn't matter.
    """
    window = get_window("blackmanharris", len(segment))
    spectrum = np.abs(np.fft.rfft(segment * window, FFT_SIZE))
    bin_hz = sample_rate / FFT_SIZE
    search = max(1, int(0.3 * fundamental_hz / bin_hz))
    levels = []
    for k in range(1, num_harmonics + 1):
        centre = int(round(k * fundamental_hz / bin_hz))
        if centre + search >= len(spectrum):
            levels.append(np.nan)
            continue
        levels.append(spectrum[centre - search:centre + search + 1].max())
    levels = np.array(levels)
    return 20 * np.log10(levels / levels[0] + 1e-12)


def thd_percent(levels_db, highest=15):
    """Total harmonic distortion from harmonics 2..highest, as a percentage of the fundamental."""
    ratios = 10 ** (np.asarray(levels_db[1:highest]) / 20)
    return 100 * np.sqrt(np.nansum(ratios ** 2))


def harmonic_map(signal, sample_rate, fundamental_hz, times_s):
    """Harmonic levels (rows = time points, columns = H1..H40) from 0.25 s windows centred on times_s."""
    half = int(WINDOW_SECONDS * sample_rate / 2)
    rows = []
    for t in times_s:
        centre = int(t * sample_rate)
        rows.append(harmonic_levels_db(signal[centre - half:centre + half], sample_rate, fundamental_hz))
    return np.array(rows)
