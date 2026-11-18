#include "TwistBell.h"

#include <cmath>

namespace rubbish
{
    void TwistBell::prepare (const juce::dsp::ProcessSpec& spec)
    {
        jassert (spec.sampleRate > 0.0 && spec.numChannels > 0);
        sampleRate = spec.sampleRate;

        // Keep clear of the top of the audio band, where the filter maths runs out of room.
        maxFrequencyHz = static_cast<float> (juce::jmin (20000.0, 0.45 * sampleRate));

        state1.assign (spec.numChannels, 0.0f);
        state2.assign (spec.numChannels, 0.0f);
        setShape (1000.0f, 0.0f, 1.0f);
    }

    void TwistBell::reset() noexcept
    {
        std::fill (state1.begin(), state1.end(), 0.0f);
        std::fill (state2.begin(), state2.end(), 0.0f);
    }

    void TwistBell::setShape (float frequencyHz, float gainDb, float q) noexcept
    {
        const auto frequency = juce::jlimit (20.0f, maxFrequencyHz, frequencyHz);
        const auto amplitude = std::pow (10.0f, gainDb / 40.0f);
        const auto g = static_cast<float> (std::tan (juce::MathConstants<double>::pi * frequency / sampleRate));
        const auto k = 1.0f / (juce::jmax (0.01f, q) * amplitude);

        a1 = 1.0f / (1.0f + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
        bandGain = k * (amplitude * amplitude - 1.0f);
    }

    float TwistBell::processSample (size_t channel, float input) noexcept
    {
        auto& s1 = state1[channel];
        auto& s2 = state2[channel];

        const auto v3 = input - s2;
        const auto band = a1 * s1 + a2 * v3;
        const auto low = s2 + a2 * s1 + a3 * v3;
        s1 = 2.0f * band - s1;
        s2 = 2.0f * low - s2;

        return input + bandGain * band;
    }
}
