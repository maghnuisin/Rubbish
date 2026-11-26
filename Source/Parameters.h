#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace rubbish
{
    namespace ParamIDs
    {
        inline constexpr auto drive     = "drive";
        inline constexpr auto push      = "push";
        inline constexpr auto character = "character";
        inline constexpr auto cleanLow  = "cleanLow";

        inline constexpr auto lowCut      = "lowCut";
        inline constexpr auto lowPeak     = "lowPeak";
        inline constexpr auto highCut     = "highCut";
        inline constexpr auto highPeak    = "highPeak";
        inline constexpr auto filterOrder = "filterOrder";
        inline constexpr auto filterFollow = "filterFollow";

        inline constexpr auto dynamicsRestore = "dynamicsRestore";
        inline constexpr auto spread      = "spread";
        inline constexpr auto autoGain    = "autoGain";
        inline constexpr auto mix         = "mix";
        inline constexpr auto output      = "output";
        inline constexpr auto delta       = "delta";

        inline constexpr auto twist     = "twist";
    }

    inline constexpr int numTwistBells = 3;

    inline constexpr int numLfos = 3;

    /** IDs for one Twist bell's parameters, e.g. "bell1Freq". bellIndex is 0-based. */
    struct TwistBellIDs
    {
        juce::String freq, gain, q;
    };

    TwistBellIDs getTwistBellIDs (int bellIndex);

    /** IDs for one LFO's own settings, e.g. "lfo1Curve". lfoIndex is 0-based. */
    struct LfoIDs
    {
        juce::String shape, curve, sync, speedSync, speedFree, start, smooth, jitter, offset, unipolar;
    };

    LfoIDs getLfoIDs (int lfoIndex);

    /** The 20 knobs an LFO can move, in a fixed order (bells first, then the panels). */
    inline constexpr int numModTargets = 20;
    const juce::StringArray& getModTargetIDs();

    /** Index into getModTargetIDs() for a bell's Freq, Gain or Q. */
    inline constexpr int bellFreqTarget (int bellIndex) noexcept { return bellIndex * 3; }
    inline constexpr int bellGainTarget (int bellIndex) noexcept { return bellIndex * 3 + 1; }
    inline constexpr int bellQTarget (int bellIndex) noexcept    { return bellIndex * 3 + 2; }

    /** The ring amount for one LFO on one knob, e.g. "lfo2ToDrive". */
    juce::String getModAmountID (int lfoIndex, int targetIndex);

    /** A ring at 100% on a bell's Freq sweeps this far each way. (5 was tried and sounded worse.) */
    inline constexpr float maxBellSweepOctaves = 3.0f;

    /** Twist's Q knob is shown as 0-100%: 0% = Q 0.20 (widest), 100% = Q 1.00 (narrowest). */
    inline constexpr float twistMinQ = 0.2f;
    inline constexpr float twistMaxQ = 1.0f;

    inline float twistQPercentToQ (float percent) noexcept
    {
        const auto proportion = juce::jlimit (0.0f, 1.0f, percent * 0.01f);
        return twistMinQ + proportion * (twistMaxQ - twistMinQ);
    }

    inline constexpr int parameterVersion = 1;
    inline constexpr int twistParameterVersion = 2; // parameters added in version 2
    inline constexpr int lfoParameterVersion = 3;   // parameters added in version 3

    /** Speed (Sync on) choices, shortest to longest. T = triplet, D = dotted.
        Must match the order of TwistClock's cycle lengths. */
    inline const juce::StringArray twistSyncSpeedNames { "1/16T", "1/16", "1/16D", "1/8T", "1/8", "1/8D",
                                                         "1/4T", "1/4", "1/4D", "1/2T", "1/2", "1/2D",
                                                         "1 bar", "2 bars", "4 bars", "8 bars" };
    inline const juce::StringArray twistShapeNames { "Smooth", "Triangle", "Ramp", "Random wander" };
    inline constexpr float pushBoostDb = 20.0f;

    /** Every plugin parameter, with ranges and defaults taken from prd.md. */
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
}
