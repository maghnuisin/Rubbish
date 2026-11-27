"""Figures for the case study. Static PNGs on a light surface, one axis per chart."""

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
from matplotlib.colors import LinearSegmentedColormap  # noqa: E402

SURFACE = "#fcfcfb"
INK = "#0b0b0b"
INK_SECONDARY = "#52514e"
GRID = "#e4e3df"
REFERENCE = INK                        # P is the target, drawn in ink so it never competes with a series hue
SERIES = ["#2a78d6", "#eb6834", "#1baf7a", "#eda100"]  # categorical slots 1-4, fixed order
LEVEL_RAMP = LinearSegmentedColormap.from_list(
    "level", ["#fcfcfb", "#cde2fb", "#86b6ef", "#3987e5", "#1c5cab", "#0d366b"])

plt.rcParams.update({
    "figure.facecolor": SURFACE, "axes.facecolor": SURFACE, "savefig.facecolor": SURFACE,
    "axes.edgecolor": GRID, "axes.labelcolor": INK_SECONDARY, "xtick.color": INK_SECONDARY,
    "ytick.color": INK_SECONDARY, "text.color": INK, "font.size": 10, "axes.titlesize": 12,
    "axes.titleweight": "bold", "axes.titlelocation": "left", "axes.grid": True,
    "grid.color": GRID, "grid.linewidth": 0.6, "axes.spines.top": False, "axes.spines.right": False,
})



def draw_thd(maps, drive, thd_curve, path, condition="44hz-push-off"):
    fig, ax = plt.subplots(figsize=(8, 4.2))
    for colour, (version, levels) in zip(SERIES, [(v, m) for v, m in maps[condition].items() if v != "Decapitator P"]):
        y = thd_curve(levels)
        ax.plot(drive, y, color=colour, linewidth=2, label=f"Grit {version}")
    p = thd_curve(maps[condition]["Decapitator P"])
    ax.plot(drive, p, color=REFERENCE, linewidth=2.5, linestyle=(0, (4, 2)), label="Decapitator P (target)")
    ax.set(xlabel="Drive (Decapitator scale; Rubbish = x3.6 dB)", ylabel="Total harmonic distortion (%)",
           xlim=(0, 10), ylim=(0, None))
    ax.set_title("Total distortion vs Drive: each Grit version against P (44 Hz sine, Push off)")
    ax.legend(frameon=False, fontsize=9, loc="upper left")
    fig.tight_layout()
    fig.savefig(path, dpi=160)
    plt.close(fig)


def draw_h2_h3(maps, drive, path, condition="44hz-push-off", version="v3 retuned to harmonic maps"):
    fig, ax = plt.subplots(figsize=(8, 4.2))
    p, g = maps[condition]["Decapitator P"], maps[condition][version]
    for column, colour, name in [(1, SERIES[0], "2nd harmonic (even: warmth, weight)"),
                                 (2, SERIES[1], "3rd harmonic (odd: edge, buzz)")]:
        ax.plot(drive, p[:, column], color=colour, linewidth=2.5, linestyle=(0, (4, 2)), label=f"P: {name}")
        ax.plot(drive, g[:, column], color=colour, linewidth=2, label=f"Grit v3: {name}")
    ax.set(xlabel="Drive (Decapitator scale)", ylabel="Level vs the note (dB)", xlim=(0, 10))
    ax.set_title("Harmonic balance: P (dashed) vs Grit v3 (solid)")
    ax.legend(frameon=False, fontsize=8.5, loc="lower right", ncol=1)
    fig.tight_layout()
    fig.savefig(path, dpi=160)
    plt.close(fig)


def draw_harmonic_maps(maps, drive, path, condition="175hz-push-off",
                       versions=("Decapitator P", "v2 refit incl. upper harmonics", "v3 retuned to harmonic maps")):
    fig, axes = plt.subplots(1, len(versions), figsize=(12, 4.4), sharey=True)
    image = None
    for ax, version in zip(axes, versions):
        levels = maps[condition][version]
        image = ax.imshow(levels[:, 1:].T, origin="lower", aspect="auto", cmap=LEVEL_RAMP, vmin=-100, vmax=-20,
                          extent=(drive[0], drive[-1], 1.5, levels.shape[1] + 0.5))
        ax.grid(False)
        ax.set_title("Decapitator P (target)" if version == "Decapitator P" else f"Grit {version}", fontsize=10.5)
        ax.set_xlabel("Drive")
    axes[0].set_ylabel("Harmonic number")
    bar = fig.colorbar(image, ax=axes, fraction=0.025, pad=0.02)
    bar.set_label("Level vs the note (dB)")
    fig.suptitle("Every harmonic up to the 40th across the Drive sweep (175 Hz note, Push off). "
                 "Pale gaps that climb with Drive are heard as a sweeping notch.",
                 x=0.06, ha="left", fontsize=11, fontweight="bold")
    fig.savefig(path, dpi=160, bbox_inches="tight")
    plt.close(fig)


def draw_all(maps, scores, drive, thd_curve, folder):
    draw_thd(maps, drive, thd_curve, folder / "thd-vs-drive.png")
    draw_h2_h3(maps, drive, folder / "h2-h3-vs-drive.png")
    draw_harmonic_maps(maps, drive, folder / "harmonic-maps-175hz.png")
