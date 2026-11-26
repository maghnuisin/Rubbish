#include "TwistStage.h"
#include "Parameters.h"

#include <cmath>

namespace rubbish
{
    namespace
    {
        float movementAt (const float* movement, size_t index) noexcept
        {
            return movement != nullptr ? movement[index] : 0.0f;
        }
    }

    void TwistStage::prepare (const juce::dsp::ProcessSpec& spec)
    {
        for (auto& voice : voices)
        {
            voice.bell.prepare (spec);
            voice.frequencyHz.reset (spec.sampleRate, smoothingSeconds);
            voice.gainDb.reset (spec.sampleRate, smoothingSeconds);
            voice.qPercent.reset (spec.sampleRate, smoothingSeconds);
        }

        wet.reset (spec.sampleRate, onOffFadeSeconds);
        reset();
    }

    // Jumps every smoothed setting straight to its target and clears the filters.
    void TwistStage::reset() noexcept
    {
        for (auto& voice : voices)
        {
            voice.bell.reset();
            voice.frequencyHz.setCurrentAndTargetValue (voice.frequencyHz.getTargetValue());
            voice.gainDb.setCurrentAndTargetValue (voice.gainDb.getTargetValue());
            voice.qPercent.setCurrentAndTargetValue (voice.qPercent.getTargetValue());
        }

        wet.setCurrentAndTargetValue (wet.getTargetValue());
        bellsNeedClearing = false;
    }

    void TwistStage::setEnabled (bool shouldBeEnabled) noexcept
    {
        wet.setTargetValue (shouldBeEnabled ? 1.0f : 0.0f);
    }

    void TwistStage::setBell (int bellIndex, const TwistBellSettings& settings) noexcept
    {
        jassert (bellIndex >= 0 && bellIndex < numBells);
        auto& voice = voices[static_cast<size_t> (juce::jlimit (0, numBells - 1, bellIndex))];

        voice.frequencyHz.setTargetValue (juce::jlimit (20.0f, 20000.0f, settings.frequencyHz));
        voice.gainDb.setTargetValue (juce::jlimit (-24.0f, 24.0f, settings.gainDb));
        voice.qPercent.setTargetValue (juce::jlimit (0.0f, qRangePercent, settings.qPercent));
    }

    bool TwistStage::isFullyOff() const noexcept
    {
        return ! wet.isSmoothing() && wet.getTargetValue() <= 0.0f;
    }

    void TwistStage::skipWhileOff() noexcept
    {
        for (auto& voice : voices)
            voice.currentFrequencyHz = voice.frequencyHz.getTargetValue();

        if (bellsNeedClearing)
        {
            for (auto& voice : voices)
                voice.bell.reset();

            bellsNeedClearing = false;
        }
    }

    // Rings can never push a bell past its own knob's range.
    void TwistStage::updateBellShape (Voice& voice, const TwistBellMovement& movement, size_t index) noexcept
    {
        const auto sweepOctaves = movementAt (movement.frequency, index) * maxBellSweepOctaves;
        const auto frequency = juce::jlimit (20.0f, voice.bell.getMaxFrequencyHz(),
                                             voice.frequencyHz.getNextValue() * std::exp2 (sweepOctaves));
        const auto gain = juce::jlimit (-24.0f, 24.0f,
                                        voice.gainDb.getNextValue() + movementAt (movement.gain, index) * gainRangeDb);
        const auto qPercent = juce::jlimit (0.0f, qRangePercent,
                                            voice.qPercent.getNextValue() + movementAt (movement.q, index) * qRangePercent);

        voice.currentFrequencyHz = frequency;
        voice.bell.setShape (frequency, gain, twistQPercentToQ (qPercent));
    }

    void TwistStage::processFrame (juce::dsp::AudioBlock<float>& block, size_t index, float wetAmount,
                                   const Movement& movement) noexcept
    {
        for (size_t bell = 0; bell < voices.size(); ++bell)
            updateBellShape (voices[bell], movement[bell], index);

        for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
        {
            auto* samples = block.getChannelPointer (ch);
            const auto dry = samples[index];
            auto twisted = dry;

            for (auto& voice : voices)
                twisted = voice.bell.processSample (ch, twisted);

            samples[index] = dry + wetAmount * (twisted - dry);
        }
    }

    float TwistStage::getCurrentFrequencyHz (int bellIndex) const noexcept
    {
        return voices[static_cast<size_t> (juce::jlimit (0, numBells - 1, bellIndex))].currentFrequencyHz;
    }

    void TwistStage::process (juce::dsp::AudioBlock<float>& block, const Movement& movement) noexcept
    {
        if (isFullyOff())
        {
            skipWhileOff();
            return;
        }

        bellsNeedClearing = true;

        for (size_t i = 0; i < block.getNumSamples(); ++i)
            processFrame (block, i, wet.getNextValue(), movement);
    }
}
