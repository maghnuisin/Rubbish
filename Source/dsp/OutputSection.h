#pragma once

#include "LoudnessMatcher.h"
#include "SafetyLimiter.h"

#include <juce_dsp/juce_dsp.h>

#include <vector>

namespace rubbish
{
    /** Everything after Dynamics Restore, in signal-flow order: Auto Gain, Mix
        (or Delta in place of both), Output level, and finally the safety limiter.

        The untouched input is delayed here to line up with the processed signal,
        so both Mix and Delta compare like with like. */
    class OutputSection
    {
    public:
        void prepare (const juce::dsp::ProcessSpec& spec, int processingLatencySamples);
        void reset();

        void setAutoGainEnabled (bool shouldBeEnabled) noexcept;
        void setMix (float wetProportion) noexcept;
        void setDeltaEnabled (bool shouldBeEnabled) noexcept;
        void setOutputDecibels (float newOutputDb) noexcept;

        /** Call with the untouched input before any processing. */
        void captureDry (const juce::dsp::AudioBlock<float>& input) noexcept;

        /** Call with the processed signal; leaves the finished output in place. */
        void process (juce::dsp::AudioBlock<float>& wet) noexcept;

    private:
        void delayDry (const juce::dsp::AudioBlock<float>& input) noexcept;
        void buildDelta (const juce::dsp::AudioBlock<float>& wet) noexcept;
        void blendIntoOutput (juce::dsp::AudioBlock<float>& wet) noexcept;

        juce::dsp::AudioBlock<float> getDryBlock (size_t numChannels, size_t numSamples) noexcept;
        juce::dsp::AudioBlock<float> getDeltaBlock (size_t numChannels, size_t numSamples) noexcept;

        static constexpr double rampSeconds = 0.05;

        // Auto Gain listens to the centre so Spread's extra width doesn't make it turn
        // the centre down (which would lose level in mono). It stays within -40..+12 dB.
        // Delta may be lifted up to +24 dB so even subtle added content is clearly audible.
        LoudnessMatcher autoGainMatcher { LoudnessMatcher::Listen::centre, -40.0f, 12.0f };
        LoudnessMatcher deltaMatcher { LoudnessMatcher::Listen::allChannels, -24.0f, 24.0f };

        juce::SmoothedValue<float> autoGainAmount { 1.0f };
        juce::SmoothedValue<float> mix { 1.0f };
        juce::SmoothedValue<float> deltaAmount { 0.0f };

        juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::None> dryDelay;
        juce::AudioBuffer<float> alignedDry;
        juce::AudioBuffer<float> delta;
        std::vector<float> autoGains;
        std::vector<float> deltaGains;

        juce::dsp::Gain<float> outputGain;
        SafetyLimiter limiter;
    };
}
