#pragma once

#include "Waveshapers.h"

#include <juce_dsp/juce_dsp.h>

namespace rubbish
{
    /** Applies the selected Character curve to every sample. When the Character
        changes, it crossfades from the old curve to the new one so switching never clicks. */
    class CharacterShaper
    {
    public:
        void prepare (double sampleRate);
        void reset();
        void setCharacter (Character newCharacter) noexcept;
        void process (juce::dsp::AudioBlock<float>& block) noexcept;

    private:
        float shapeSample (float x, float fadeToCurrent) const noexcept;

        static constexpr double crossfadeSeconds = 0.03;

        Character current = Character::tape;
        Character previous = Character::tape;
        juce::SmoothedValue<float> crossfade { 1.0f };
    };
}
