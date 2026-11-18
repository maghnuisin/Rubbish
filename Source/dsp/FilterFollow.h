#pragma once

#include <juce_dsp/juce_dsp.h>

#include <vector>

namespace rubbish
{
    /** Filter Follow: turns the input's loudness into a per-sample High Cut
        movement. Positive amounts open the filter on loud hits, negative close it. */
    class FilterFollow
    {
    public:
        void prepare (double sampleRate, size_t maximumBlockSize);
        void reset();

        void setAmount (float newAmount) noexcept; // -1..+1

        /** Fills and returns one cutoff multiplier per sample (1 = no movement). */
        const float* computeCutoffMultipliers (const float* loudness, size_t numSamples) noexcept;

    private:
        static float loudnessToOctaves (float loudness) noexcept;

        // At 100%, a full-scale input moves the cutoff this many octaves.
        static constexpr float maxOctaves = 4.0f;
        static constexpr float quietestDb = -48.0f;
        static constexpr double smoothingSeconds = 0.05;

        juce::SmoothedValue<float> amount { 0.0f };
        std::vector<float> multipliers;
    };
}
