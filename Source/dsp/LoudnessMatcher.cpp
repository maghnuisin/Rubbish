#include "LoudnessMatcher.h"

#include <cmath>

namespace rubbish
{
    namespace
    {
        // Below about -60 dBFS RMS the reference counts as silence and the gain is held.
        constexpr float silenceMeanSquare = 1.0e-6f;

        float onePoleCoefficient (double seconds, double sampleRate)
        {
            return static_cast<float> (1.0 - std::exp (-1.0 / (seconds * sampleRate)));
        }
    }

    LoudnessMatcher::LoudnessMatcher (Listen whatToListenTo, float minimumGainDb, float maximumGainDb)
        : listen (whatToListenTo),
          minimumGain (juce::Decibels::decibelsToGain (minimumGainDb)),
          maximumGain (juce::Decibels::decibelsToGain (maximumGainDb))
    {
        jassert (minimumGainDb < maximumGainDb);
    }

    void LoudnessMatcher::prepare (double sampleRate)
    {
        jassert (sampleRate > 0.0);
        averagingCoefficient = onePoleCoefficient (averagingSeconds, sampleRate);
        gainSmoothingCoefficient = onePoleCoefficient (gainSmoothingSeconds, sampleRate);
        reset();
    }

    void LoudnessMatcher::reset()
    {
        referenceMeanSquare = 0.0f;
        targetMeanSquare = 0.0f;
        heldGain = 1.0f;
        smoothedGain = 1.0f;
    }

    void LoudnessMatcher::measureReference (const juce::dsp::AudioBlock<float>& reference) noexcept
    {
        for (size_t i = 0; i < reference.getNumSamples(); ++i)
            referenceMeanSquare += averagingCoefficient * (meanSquareAt (reference, i) - referenceMeanSquare);
    }

    void LoudnessMatcher::computeGains (const juce::dsp::AudioBlock<float>& target, float* gainsOut) noexcept
    {
        for (size_t i = 0; i < target.getNumSamples(); ++i)
            gainsOut[i] = nextGain (meanSquareAt (target, i));
    }

    float LoudnessMatcher::nextGain (float targetSampleMeanSquare) noexcept
    {
        targetMeanSquare += averagingCoefficient * (targetSampleMeanSquare - targetMeanSquare);

        if (referenceMeanSquare > silenceMeanSquare && targetMeanSquare > 0.0f)
            heldGain = juce::jlimit (minimumGain, maximumGain, std::sqrt (referenceMeanSquare / targetMeanSquare));

        smoothedGain += gainSmoothingCoefficient * (heldGain - smoothedGain);
        return smoothedGain;
    }

    float LoudnessMatcher::meanSquareAt (const juce::dsp::AudioBlock<float>& block, size_t index) const noexcept
    {
        if (listen == Listen::centre && block.getNumChannels() == 2)
            return centreSquared (block, index);

        return meanSquareAcrossChannels (block, index);
    }

    float LoudnessMatcher::centreSquared (const juce::dsp::AudioBlock<float>& block, size_t index) noexcept
    {
        const auto centre = 0.5f * (block.getChannelPointer (0)[index] + block.getChannelPointer (1)[index]);
        return centre * centre;
    }

    float LoudnessMatcher::meanSquareAcrossChannels (const juce::dsp::AudioBlock<float>& block, size_t index) noexcept
    {
        float sum = 0.0f;

        for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
        {
            const auto sample = block.getChannelPointer (ch)[index];
            sum += sample * sample;
        }

        const auto numChannels = block.getNumChannels() > 0 ? block.getNumChannels() : size_t { 1 };
        return sum / static_cast<float> (numChannels);
    }
}
