#include "FilterFollow.h"

#include <cmath>

namespace rubbish
{
    void FilterFollow::prepare (double sampleRate, size_t maximumBlockSize)
    {
        jassert (sampleRate > 0.0 && maximumBlockSize > 0);
        amount.reset (sampleRate, smoothingSeconds);
        multipliers.assign (maximumBlockSize, 1.0f);
        reset();
    }

    void FilterFollow::reset()
    {
        amount.setCurrentAndTargetValue (amount.getTargetValue());
        std::fill (multipliers.begin(), multipliers.end(), 1.0f);
    }

    void FilterFollow::setAmount (float newAmount) noexcept
    {
        amount.setTargetValue (juce::jlimit (-1.0f, 1.0f, newAmount));
    }

    const float* FilterFollow::computeCutoffMultipliers (const float* loudness, size_t numSamples) noexcept
    {
        jassert (numSamples <= multipliers.size());
        numSamples = juce::jmin (numSamples, multipliers.size());

        for (size_t i = 0; i < numSamples; ++i)
        {
            const auto octaves = amount.getNextValue() * loudnessToOctaves (loudness[i]);
            multipliers[i] = std::exp2 (octaves);
        }

        return multipliers.data();
    }

    // Works in decibels so the movement feels even across quiet and loud playing.
    float FilterFollow::loudnessToOctaves (float loudness) noexcept
    {
        const auto loudnessDb = juce::Decibels::gainToDecibels (loudness, quietestDb);
        const auto proportion = juce::jlimit (0.0f, 1.0f, (loudnessDb - quietestDb) / -quietestDb);
        return proportion * maxOctaves;
    }
}
