#include "TwistLfo.h"

#include <cmath>

namespace rubbish
{
    namespace
    {
        double fractionalPart (double value) noexcept
        {
            return value - std::floor (value);
        }

        // Starts at 0 and rises first, like the smooth wave.
        float triangleAt (double phase) noexcept
        {
            const auto t = static_cast<float> (phase);

            if (t < 0.25f)
                return 4.0f * t;

            if (t < 0.75f)
                return 2.0f - 4.0f * t;

            return 4.0f * t - 4.0f;
        }
    }

    void TwistLfo::prepare (double sampleRate)
    {
        jassert (sampleRate > 0.0);
        smoothingCoefficient = static_cast<float> (std::exp (-1.0 / (smoothingSeconds * sampleRate)));
        reset();
    }

    void TwistLfo::reset() noexcept
    {
        hasStarted = false;
        smoothedValue = 0.0f;
    }

    float TwistLfo::next (double cyclePosition) noexcept
    {
        const auto target = shapeAt (cyclePosition);

        if (! hasStarted)
        {
            smoothedValue = target;
            hasStarted = true;
        }

        smoothedValue = target + smoothingCoefficient * (smoothedValue - target);
        return smoothedValue;
    }

    float TwistLfo::shapeAt (double cyclePosition) const noexcept
    {
        const auto phase = fractionalPart (cyclePosition);

        switch (shape)
        {
            case TwistShape::triangle:     return triangleAt (phase);
            case TwistShape::ramp:         return static_cast<float> (2.0 * phase - 1.0);
            case TwistShape::randomWander: return randomWanderAt (cyclePosition);
            case TwistShape::smooth:       break;
        }

        return static_cast<float> (std::sin (juce::MathConstants<double>::twoPi * phase));
    }

    // Glides smoothly from one random point to the next, one point per cycle.
    // The points come from the cycle number, so the same bar always wanders the same way.
    float TwistLfo::randomWanderAt (double cyclePosition) const noexcept
    {
        const auto cycleIndex = static_cast<juce::int64> (std::floor (cyclePosition));
        const auto blend = 0.5f - 0.5f * std::cos (juce::MathConstants<float>::pi
                                                   * static_cast<float> (fractionalPart (cyclePosition)));
        const auto from = randomPointFor (cycleIndex);
        const auto to = randomPointFor (cycleIndex + 1);
        return from + (to - from) * blend;
    }

    float TwistLfo::randomPointFor (juce::int64 cycleIndex) const noexcept
    {
        // A small integer hash (splitmix64): repeatable, allocation-free, and well spread.
        auto x = static_cast<juce::uint64> (cycleIndex) + 0x9e3779b97f4a7c15ull * static_cast<juce::uint64> (seed + 1);
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ull;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebull;
        x = x ^ (x >> 31);

        const auto unit = static_cast<float> (x >> 40) / static_cast<float> (1ull << 24); // 0..1
        return unit * 2.0f - 1.0f;
    }
}
