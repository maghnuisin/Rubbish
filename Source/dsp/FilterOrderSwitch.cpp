#include "FilterOrderSwitch.h"

namespace rubbish
{
    void FilterOrderSwitch::prepare (double sampleRate)
    {
        jassert (sampleRate > 0.0);
        fadeGain.reset (sampleRate, fadeSeconds);
    }

    void FilterOrderSwitch::reset (FilterOrder order)
    {
        active = order;
        fadeGain.setCurrentAndTargetValue (1.0f);
    }

    void FilterOrderSwitch::setRequestedOrder (FilterOrder requested) noexcept
    {
        if (requested == active)
        {
            fadeGain.setTargetValue (1.0f);
            return;
        }

        if (isSilent())
        {
            active = requested;
            fadeGain.setTargetValue (1.0f);
            return;
        }

        fadeGain.setTargetValue (0.0f);
    }

    bool FilterOrderSwitch::isSilent() const noexcept
    {
        return ! fadeGain.isSmoothing() && fadeGain.getCurrentValue() <= 0.0f;
    }

    void FilterOrderSwitch::applyFade (juce::dsp::AudioBlock<float>& block) noexcept
    {
        if (! fadeGain.isSmoothing())
        {
            if (fadeGain.getCurrentValue() < 1.0f)
                block.multiplyBy (fadeGain.getCurrentValue());
            return;
        }

        for (size_t i = 0; i < block.getNumSamples(); ++i)
        {
            const auto gain = fadeGain.getNextValue();

            for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
                block.getChannelPointer (ch)[i] *= gain;
        }
    }
}
