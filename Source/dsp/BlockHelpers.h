#pragma once

#include <juce_dsp/juce_dsp.h>

#include <cmath>

namespace rubbish
{
    /** The loudest absolute sample across all channels at one position in a block. */
    inline float loudestSampleAt (const juce::dsp::AudioBlock<float>& block, size_t index) noexcept
    {
        float loudest = 0.0f;

        for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
            loudest = juce::jmax (loudest, std::abs (block.getChannelPointer (ch)[index]));

        return loudest;
    }
}
