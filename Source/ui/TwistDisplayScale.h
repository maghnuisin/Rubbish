#pragma once

#include <juce_graphics/juce_graphics.h>

#include <cmath>

namespace rubbish::twistDisplayScale
{
    inline constexpr float minFrequencyHz = 20.0f;
    inline constexpr float maxFrequencyHz = 20000.0f;
    inline constexpr float maxGainDb = 24.0f;

    /** Frequencies are spread evenly by octave, like an EQ display. */
    inline float frequencyToX (float frequencyHz, juce::Rectangle<float> area) noexcept
    {
        const auto proportion = std::log (frequencyHz / minFrequencyHz) / std::log (maxFrequencyHz / minFrequencyHz);
        return area.getX() + proportion * area.getWidth();
    }

    inline float xToFrequency (float x, juce::Rectangle<float> area) noexcept
    {
        const auto proportion = juce::jlimit (0.0f, 1.0f, (x - area.getX()) / area.getWidth());
        return minFrequencyHz * std::pow (maxFrequencyHz / minFrequencyHz, proportion);
    }

    inline float gainToY (float gainDb, juce::Rectangle<float> area) noexcept
    {
        return area.getCentreY() - (gainDb / maxGainDb) * area.getHeight() * 0.5f;
    }

    inline float yToGain (float y, juce::Rectangle<float> area) noexcept
    {
        const auto gain = (area.getCentreY() - y) / (area.getHeight() * 0.5f) * maxGainDb;
        return juce::jlimit (-maxGainDb, maxGainDb, gain);
    }

    /** How much one bell boosts or cuts at a frequency, in dB. Matches TwistBell's
        filter: H(s) = (s^2 + k*A^2*s + 1) / (s^2 + k*s + 1), with k = 1 / (Q * A). */
    inline float bellResponseDb (float frequencyHz, float centreHz, float gainDb, float q) noexcept
    {
        const auto amplitude = std::pow (10.0f, gainDb / 40.0f);
        const auto k = 1.0f / (juce::jmax (0.01f, q) * amplitude);
        const auto w = frequencyHz / centreHz;
        const auto real = 1.0f - w * w;
        const auto numeratorImag = k * amplitude * amplitude * w;
        const auto denominatorImag = k * w;

        const auto numerator = real * real + numeratorImag * numeratorImag;
        const auto denominator = real * real + denominatorImag * denominatorImag;
        return 10.0f * std::log10 (juce::jmax (1.0e-12f, numerator) / juce::jmax (1.0e-12f, denominator));
    }
}
