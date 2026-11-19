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
        inline constexpr auto twistSync = "twistSync";
    }

    inline constexpr int numTwistBells = 3;

    /** IDs for one Twist bell's parameters, e.g. "bell1Freq". bellIndex is 0-based. */
    struct TwistBellIDs
    {
        juce::String freq, gain, q, shape, speedSync, speedFree, move, start;
    };

    TwistBellIDs getTwistBellIDs (int bellIndex);

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
