#include "CharacterShaper.h"

#include <algorithm>
#include <cmath>

namespace rubbish
{
    void CharacterShaper::prepare (double sampleRate, int numChannels)
    {
        jassert (sampleRate > 0.0 && numChannels > 0);
        crossfade.reset (sampleRate, crossfadeSeconds);

        for (auto* smoother : { &driveDb, &totalDb, &push })
            smoother->reset (sampleRate, driveGlideSeconds);

        peaks.assign (static_cast<size_t> (numChannels), 0.0f);
        peakRelease = static_cast<float> (std::exp (-1.0 / (peakReleaseSeconds * sampleRate)));
        reset();
    }

    void CharacterShaper::reset()
    {
        previous = current;
        crossfade.setCurrentAndTargetValue (1.0f);

        for (auto* smoother : { &driveDb, &totalDb, &push })
            smoother->setCurrentAndTargetValue (smoother->getTargetValue());

        std::fill (peaks.begin(), peaks.end(), 0.0f);
        refreshCurveSettings (0);
        samplesUntilSettingsUpdate = samplesPerSettingsUpdate;
    }

    void CharacterShaper::setCharacter (Character newCharacter) noexcept
    {
        if (newCharacter == current)
            return;

        previous = current;
        current = newCharacter;
        crossfade.setCurrentAndTargetValue (0.0f);
        crossfade.setTargetValue (1.0f);
        refreshCurveSettings (0);
    }

    void CharacterShaper::setDrive (const DriveState& newDrive) noexcept
    {
        driveDb.setTargetValue (newDrive.driveDb);
        totalDb.setTargetValue (newDrive.totalDb);
        push.setTargetValue (newDrive.push);
    }

    // Working the curve settings out every few samples (rather than every sample)
    // keeps the cost low; the glide is far slower than the update rate, so it's inaudible.
    void CharacterShaper::refreshCurveSettings (int samplesToAdvance) noexcept
    {
        // skip (0) would jump straight to the target, so read the current value instead.
        const auto advance = [samplesToAdvance] (juce::SmoothedValue<float>& value)
        {
            return samplesToAdvance > 0 ? value.skip (samplesToAdvance) : value.getCurrentValue();
        };

        const DriveState drive { advance (driveDb), advance (totalDb), advance (push) };

        currentSettings = computeCurveSettings (current, drive);
        previousSettings = computeCurveSettings (previous, drive);
    }

    void CharacterShaper::process (juce::dsp::AudioBlock<float>& block) noexcept
    {
        const auto numChannels = juce::jmin (block.getNumChannels(), peaks.size());
        const auto numSamples = block.getNumSamples();

        for (size_t i = 0; i < numSamples; ++i)
        {
            if (--samplesUntilSettingsUpdate <= 0)
            {
                refreshCurveSettings (samplesPerSettingsUpdate);
                samplesUntilSettingsUpdate = samplesPerSettingsUpdate;
            }

            const auto fade = crossfade.getNextValue();

            for (size_t ch = 0; ch < numChannels; ++ch)
            {
                auto* data = block.getChannelPointer (ch);
                const auto peak = trackPeak (ch, data[i]);
                data[i] = shapeSample (data[i], peak, fade);
            }
        }
    }

    // Jumps up instantly and falls back slowly, so it holds across a whole bass cycle.
    float CharacterShaper::trackPeak (size_t channel, float x) noexcept
    {
        auto& peak = peaks[channel];
        peak = juce::jmax (std::abs (x), peak * peakRelease);
        return peak;
    }

    float CharacterShaper::shapeSample (float x, float peak, float fadeToCurrent) const noexcept
    {
        const auto currentOut = waveshapers::shape (x, currentSettings, peak);

        if (fadeToCurrent >= 1.0f)
            return currentOut;

        const auto previousOut = waveshapers::shape (x, previousSettings, peak);
        return previousOut + fadeToCurrent * (currentOut - previousOut);
    }
}
