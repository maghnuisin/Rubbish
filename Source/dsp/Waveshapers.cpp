#include "Waveshapers.h"

#include <cmath>

namespace rubbish::waveshapers
{
    // Smooth, gradual rounding of peaks: the classic warm tape curve.
    float tape (float x) noexcept
    {
        return std::tanh (x);
    }

    // Stays clean longer, then bends over sharply. That later, firmer knee
    // keeps transients forward and punchy.
    float console (float x) noexcept
    {
        constexpr float kneeEnd = 1.5f;

        if (x >= kneeEnd)  return 1.0f;
        if (x <= -kneeEnd) return -1.0f;

        const auto scaled = x / kneeEnd;
        return 1.5f * scaled - 0.5f * scaled * scaled * scaled;
    }

    // Biasing the curve off-centre makes the two halves of the wave clip
    // differently. That asymmetry creates the even harmonics heard as "thick".
    // The DC offset this introduces is removed after the shaper.
    float tube (float x) noexcept
    {
        constexpr float bias = 0.35f;
        static const float biasOffset = std::tanh (bias);
        static const float normaliser = 1.0f / (1.0f + biasOffset);
        static const float smallSignalGain = (1.0f - biasOffset * biasOffset) * normaliser;

        // Pre-scaling the input keeps quiet signals at their original level.
        const auto unityInput = x / smallSignalGain;
        return (std::tanh (unityInput + bias) - biasOffset) * normaliser;
    }

    // A very hard knee, close to a straight clip: raw and biting.
    float synth (float x) noexcept
    {
        constexpr float hardness = 8.0f;
        return x / std::pow (1.0f + std::pow (std::abs (x), hardness), 1.0f / hardness);
    }

    float apply (Character character, float x) noexcept
    {
        switch (character)
        {
            case Character::tape:    return tape (x);
            case Character::console: return console (x);
            case Character::tube:    return tube (x);
            case Character::synth:   return synth (x);
        }

        return x;
    }
}
