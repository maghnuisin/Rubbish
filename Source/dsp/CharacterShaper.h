#pragma once

#include "CharacterVoicing.h"

#include <juce_dsp/juce_dsp.h>

#include <vector>

namespace rubbish
{
    /** Applies the selected Character curve to every sample. The curve follows Drive
        and Push (Push makes it harder as well as hotter), so the sound changes character
        as Drive is turned, not just level. Changing Character crossfades from the old
        curve to the new one, and Drive and Push glide, so nothing ever clicks. */
    class CharacterShaper
    {
    public:
        void prepare (double sampleRate, int numChannels);
        void reset();
        void setCharacter (Character newCharacter) noexcept;
        void setDrive (const DriveState& newDrive) noexcept;
        void process (juce::dsp::AudioBlock<float>& block) noexcept;

    private:
        void refreshCurveSettings (int samplesToAdvance) noexcept;
        float trackPeak (size_t channel, float x) noexcept;
        float shapeSample (float x, float peak, float fadeToCurrent) const noexcept;

        static constexpr double crossfadeSeconds = 0.03;
        static constexpr double driveGlideSeconds = 0.05; // matches the drive gain ramp
        static constexpr double peakReleaseSeconds = 0.1;
        static constexpr int samplesPerSettingsUpdate = 16;

        Character current = Character::tape;
        Character previous = Character::tape;
        juce::SmoothedValue<float> crossfade { 1.0f };

        juce::SmoothedValue<float> driveDb, totalDb, push;
        CurveSettings currentSettings, previousSettings;
        int samplesUntilSettingsUpdate = 0;

        std::vector<float> peaks;
        float peakRelease = 0.0f;
    };
}
