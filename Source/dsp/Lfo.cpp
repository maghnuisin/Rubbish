#include "Lfo.h"

#include <cmath>

namespace rubbish
{
    namespace
    {
        double fractionalPart (double value) noexcept
        {
            return value - std::floor (value);
        }

        // Curve moves the peak: positive makes the rise shorter than the fall.
        // At 0 the peak sits half way, which gives exactly version 2's waves.
        double riseFraction (float curve) noexcept
        {
            return 0.5 - 0.45 * static_cast<double> (juce::jlimit (-1.0f, 1.0f, curve));
        }

        // Maps time since the lowest point to 0..0.5 while rising and 0.5..1 while falling.
        double warpFromTrough (double sinceTrough, double rise) noexcept
        {
            if (sinceTrough < rise)
                return 0.5 * sinceTrough / rise;

            return 0.5 + 0.5 * (sinceTrough - rise) / (1.0 - rise);
        }
    }

    void Lfo::prepare (double newSampleRate)
    {
        jassert (newSampleRate > 0.0);
        sampleRate = newSampleRate;
        setShape (settings, 1.0);
        reset();
    }

    void Lfo::reset() noexcept
    {
        hasStarted = false;
        smoothedValue = 0.0f;
    }

    void Lfo::setShape (const LfoShapeSettings& newSettings, double cycleSeconds) noexcept
    {
        settings = newSettings;
        const auto smoothingSeconds = minimumSmoothingSeconds
                                      + static_cast<double> (juce::jlimit (0.0f, 1.0f, settings.smooth))
                                          * maximumSmoothingCycles * juce::jmax (0.0, cycleSeconds);
        smoothingCoefficient = static_cast<float> (std::exp (-1.0 / (smoothingSeconds * sampleRate)));
    }

    float Lfo::next (double cyclePosition) noexcept
    {
        const auto target = applyOffsetAndPolarity (shapeAt (cyclePosition) + settings.jitter * jitterAt (cyclePosition));

        if (! hasStarted)
        {
            smoothedValue = target;
            hasStarted = true;
        }

        smoothedValue = target + smoothingCoefficient * (smoothedValue - target);
        return smoothedValue;
    }

    float Lfo::applyOffsetAndPolarity (float value) const noexcept
    {
        const auto bipolar = juce::jlimit (-1.0f, 1.0f, value + settings.offset);
        return settings.unipolar ? 0.5f * (bipolar + 1.0f) : bipolar;
    }

    float Lfo::shapeAt (double cyclePosition) const noexcept
    {
        if (settings.shape == TwistShape::randomWander)
            return randomWanderAt (cyclePosition); // Curve doesn't apply to a random drift

        return curvedWaveAt (fractionalPart (cyclePosition));
    }

    float Lfo::curvedWaveAt (double phase) const noexcept
    {
        if (settings.shape == TwistShape::ramp)
        {
            // Positive Curve bends the ramp so it climbs quickly then eases in; negative the reverse.
            const auto bend = std::exp2 (-2.0 * static_cast<double> (settings.curve));
            return static_cast<float> (2.0 * std::pow (phase, bend) - 1.0);
        }

        // Smooth and Triangle start at the middle going up; their lowest point is three quarters in.
        const auto warped = warpFromTrough (fractionalPart (phase + 0.25), riseFraction (settings.curve));

        if (settings.shape == TwistShape::triangle)
            return static_cast<float> (1.0 - 4.0 * std::abs (warped - 0.5));

        return static_cast<float> (-std::cos (juce::MathConstants<double>::twoPi * warped));
    }

    // Glides smoothly from one random point to the next, one point per cycle.
    float Lfo::randomWanderAt (double cyclePosition) const noexcept
    {
        const auto cycleIndex = static_cast<juce::int64> (std::floor (cyclePosition));
        const auto blend = 0.5f - 0.5f * std::cos (juce::MathConstants<float>::pi
                                                   * static_cast<float> (fractionalPart (cyclePosition)));
        const auto from = randomPointFor (cycleIndex, 0);
        const auto to = randomPointFor (cycleIndex + 1, 0);
        return from + (to - from) * blend;
    }

    // Rough random steps, several per cycle; Smooth rounds them off.
    float Lfo::jitterAt (double cyclePosition) const noexcept
    {
        const auto step = static_cast<juce::int64> (std::floor (cyclePosition * jitterPointsPerCycle));
        return randomPointFor (step, 1);
    }

    float Lfo::randomPointFor (juce::int64 index, int stream) const noexcept
    {
        // A small integer hash (splitmix64): repeatable, allocation-free, and well spread.
        // Stream 0 keeps version 2's Random wander points exactly.
        auto x = static_cast<juce::uint64> (index)
                 + 0x9e3779b97f4a7c15ull * static_cast<juce::uint64> (seed + 1 + 1000 * stream);
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ull;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebull;
        x = x ^ (x >> 31);

        const auto unit = static_cast<float> (x >> 40) / static_cast<float> (1ull << 24); // 0..1
        return unit * 2.0f - 1.0f;
    }
}
