#pragma once

#include <juce_dsp/juce_dsp.h>

#include <vector>

namespace rubbish
{
    /** Follows how loud the clean input is, moment to moment. This one tracker is
        shared by Dynamics Restore and Filter Follow so they react identically. */
    class LoudnessTracker
    {
    public:
        // Fast enough to catch a drum hit, slow enough not to wobble on bass notes.
        static constexpr float attackMs = 1.0f;
        static constexpr float releaseMs = 80.0f;

        /** Configures a ballistics filter with this tracker's fixed reaction speed. */
        static void configure (juce::dsp::BallisticsFilter<float>& follower, double sampleRate);

        void prepare (const juce::dsp::ProcessSpec& spec);
        void reset();

        /** Reads the input and fills one loudness value (linear, 0..) per sample. */
        void process (const juce::dsp::AudioBlock<float>& input) noexcept;

        /** Loudness per sample for the block last passed to process(). */
        const float* getEnvelope() const noexcept { return envelope.data(); }

    private:
        juce::dsp::BallisticsFilter<float> follower;
        std::vector<float> envelope;
    };
}
