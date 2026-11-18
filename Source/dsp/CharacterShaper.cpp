#include "CharacterShaper.h"

namespace rubbish
{
    void CharacterShaper::prepare (double sampleRate)
    {
        jassert (sampleRate > 0.0);
        crossfade.reset (sampleRate, crossfadeSeconds);
        reset();
    }

    void CharacterShaper::reset()
    {
        previous = current;
        crossfade.setCurrentAndTargetValue (1.0f);
    }

    void CharacterShaper::setCharacter (Character newCharacter) noexcept
    {
        if (newCharacter == current)
            return;

        previous = current;
        current = newCharacter;
        crossfade.setCurrentAndTargetValue (0.0f);
        crossfade.setTargetValue (1.0f);
    }

    void CharacterShaper::process (juce::dsp::AudioBlock<float>& block) noexcept
    {
        const auto numChannels = block.getNumChannels();
        const auto numSamples = block.getNumSamples();

        for (size_t i = 0; i < numSamples; ++i)
        {
            const auto fade = crossfade.getNextValue();

            for (size_t ch = 0; ch < numChannels; ++ch)
            {
                auto* data = block.getChannelPointer (ch);
                data[i] = shapeSample (data[i], fade);
            }
        }
    }

    float CharacterShaper::shapeSample (float x, float fadeToCurrent) const noexcept
    {
        const auto currentOut = waveshapers::apply (current, x);

        if (fadeToCurrent >= 1.0f)
            return currentOut;

        const auto previousOut = waveshapers::apply (previous, x);
        return previousOut + fadeToCurrent * (currentOut - previousOut);
    }
}
