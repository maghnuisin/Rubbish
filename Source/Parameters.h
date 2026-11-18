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
    }

    inline constexpr int parameterVersion = 1;
    inline constexpr float pushBoostDb = 20.0f;

    /** Every plugin parameter, with ranges and defaults taken from prd.md. */
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
}
