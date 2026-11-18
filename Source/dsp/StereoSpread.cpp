#include "StereoSpread.h"

namespace rubbish
{
    void StereoSpread::prepare (double oversampledRate, size_t maxOversampledBlockSize)
    {
        jassert (oversampledRate > 0.0 && maxOversampledBlockSize > 0);
        sideShaper.prepare (oversampledRate);
        sides.setSize (1, static_cast<int> (maxOversampledBlockSize));
        amount.reset (oversampledRate, smoothingSeconds);
        reset();
    }

    void StereoSpread::reset()
    {
        sideShaper.reset();
        sides.clear();
        capturedSamples = 0;
        amount.setCurrentAndTargetValue (amount.getTargetValue());
    }

    void StereoSpread::setAmount (float newAmount) noexcept
    {
        amount.setTargetValue (juce::jlimit (0.0f, 1.0f, newAmount));
    }

    void StereoSpread::setCharacter (Character newCharacter) noexcept
    {
        sideShaper.setCharacter (newCharacter);
    }

    bool StereoSpread::isStereo (const juce::dsp::AudioBlock<float>& block) noexcept
    {
        return block.getNumChannels() == 2;
    }

    void StereoSpread::captureSides (const juce::dsp::AudioBlock<float>& driven) noexcept
    {
        capturedSamples = 0;

        if (! isStereo (driven))
            return;

        jassert (driven.getNumSamples() <= static_cast<size_t> (sides.getNumSamples()));
        capturedSamples = juce::jmin (driven.getNumSamples(), static_cast<size_t> (sides.getNumSamples()));

        const auto* left = driven.getChannelPointer (0);
        const auto* right = driven.getChannelPointer (1);
        auto* side = sides.getWritePointer (0);

        for (size_t i = 0; i < capturedSamples; ++i)
            side[i] = 0.5f * (left[i] - right[i]) * sideDriveBoost;

        juce::dsp::AudioBlock<float> sideBlock (sides);
        auto activePart = sideBlock.getSubBlock (0, capturedSamples);
        sideShaper.process (activePart);
    }

    void StereoSpread::apply (juce::dsp::AudioBlock<float>& shaped) noexcept
    {
        if (! isStereo (shaped) || capturedSamples != shaped.getNumSamples())
            return;

        auto* left = shaped.getChannelPointer (0);
        auto* right = shaped.getChannelPointer (1);
        const auto* hardSide = sides.getReadPointer (0);

        for (size_t i = 0; i < capturedSamples; ++i)
        {
            const auto mid = 0.5f * (left[i] + right[i]);
            const auto side = 0.5f * (left[i] - right[i]);
            const auto newSide = side + amount.getNextValue() * (hardSide[i] - side);

            left[i] = mid + newSide;
            right[i] = mid - newSide;
        }
    }
}
