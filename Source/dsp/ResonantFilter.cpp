#include "ResonantFilter.h"

#include <cmath>

namespace rubbish
{
    namespace
    {
        constexpr float minimumQ = 0.7071f; // flat (Butterworth) response at 0% Peak
        constexpr float maximumQ = 40.0f;   // sharp howl at 100% Peak
        constexpr float stateLimit = 3.0f;  // where the resonance starts to be reined in

        // Keeps the highest cutoff safely below the point where the filter maths breaks down.
        constexpr double maxCutoffFractionOfSampleRate = 0.45;
    }

    ResonantFilter::ResonantFilter (FilterType filterType)
        : type (filterType)
    {
    }

    void ResonantFilter::prepare (const juce::dsp::ProcessSpec& spec)
    {
        jassert (spec.sampleRate > 0.0 && spec.numChannels > 0);

        sampleRate = spec.sampleRate;
        maxCutoffHz = static_cast<float> (sampleRate * maxCutoffFractionOfSampleRate);

        cutoffHz.reset (sampleRate, smoothingSeconds);
        peak.reset (sampleRate, smoothingSeconds);

        bandState.assign (spec.numChannels, 0.0f);
        lowState.assign (spec.numChannels, 0.0f);
    }

    void ResonantFilter::reset()
    {
        cutoffHz.setCurrentAndTargetValue (cutoffHz.getTargetValue());
        peak.setCurrentAndTargetValue (peak.getTargetValue());
        std::fill (bandState.begin(), bandState.end(), 0.0f);
        std::fill (lowState.begin(), lowState.end(), 0.0f);
    }

    void ResonantFilter::setCutoffHz (float newCutoffHz) noexcept
    {
        cutoffHz.setTargetValue (juce::jlimit (10.0f, maxCutoffHz, newCutoffHz));
    }

    void ResonantFilter::setPeak (float newPeak) noexcept
    {
        peak.setTargetValue (juce::jlimit (0.0f, 1.0f, newPeak));
    }

    void ResonantFilter::process (juce::dsp::AudioBlock<float>& block, const float* cutoffMultipliers) noexcept
    {
        const auto numChannels = juce::jmin (block.getNumChannels(), bandState.size());
        jassert (numChannels == block.getNumChannels()); // prepare() was given too few channels

        for (size_t i = 0; i < block.getNumSamples(); ++i)
        {
            const auto coefficients = nextCoefficients (cutoffMultipliers != nullptr ? cutoffMultipliers[i] : 1.0f);

            for (size_t ch = 0; ch < numChannels; ++ch)
            {
                auto* data = block.getChannelPointer (ch);
                data[i] = processSample (ch, data[i], coefficients);
            }
        }
    }

    ResonantFilter::Coefficients ResonantFilter::nextCoefficients (float cutoffMultiplier) noexcept
    {
        Coefficients c;
        const auto cutoff = static_cast<double> (juce::jlimit (10.0f, maxCutoffHz, cutoffHz.getNextValue() * cutoffMultiplier));
        c.g = static_cast<float> (std::tan (juce::MathConstants<double>::pi * cutoff / sampleRate));
        c.damping = peakToDamping (peak.getNextValue());
        c.normaliser = 1.0f / (1.0f + c.damping * c.g + c.g * c.g);
        return c;
    }

    float ResonantFilter::processSample (size_t channel, float input, const Coefficients& c) noexcept
    {
        auto& s1 = bandState[channel];
        auto& s2 = lowState[channel];

        const auto high = (input - (c.damping + c.g) * s1 - s2) * c.normaliser;
        const auto band = c.g * high + s1;
        const auto low = c.g * band + s2;

        s1 = limitState (c.g * high + band);
        s2 = limitState (c.g * band + low);

        return type == FilterType::lowPass ? low : high;
    }

    // Peak sweeps Q on a logarithmic scale, so low settings give a gentle bump
    // and the top of the range gets dramatic.
    float ResonantFilter::peakToDamping (float peakAmount) noexcept
    {
        const auto q = minimumQ * std::pow (maximumQ / minimumQ, peakAmount);
        return 1.0f / q;
    }

    // Practically transparent at normal levels, then firmly caps the stored energy.
    // This is the filter's version of the MS-20's diode limiting.
    float ResonantFilter::limitState (float state) noexcept
    {
        const auto ratio = state / stateLimit;
        const auto ratioSquared = ratio * ratio;
        return state / std::sqrt (std::sqrt (1.0f + ratioSquared * ratioSquared));
    }
}
