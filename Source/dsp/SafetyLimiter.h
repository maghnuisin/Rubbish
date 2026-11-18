#pragma once

#include <juce_dsp/juce_dsp.h>

namespace rubbish
{
    /** The last thing in the plugin. Does nothing to signals below its ceiling.
        When a peak would go over, it turns the volume down instantly, then lets
        it recover smoothly. A final hard ceiling guarantees nothing exceeds 0 dBFS. */
    class SafetyLimiter
    {
    public:
        void prepare (double sampleRate);
        void reset();
        void process (juce::dsp::AudioBlock<float>& block) noexcept;

    private:
        float nextGain (float peak) noexcept;

        static constexpr double releaseSeconds = 0.1;

        float releaseMultiplier = 0.0f;
        float gainReduction = 0.0f;
    };
}
