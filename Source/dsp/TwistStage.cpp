#include "TwistStage.h"

#include <cmath>

namespace rubbish
{
    void TwistStage::prepare (const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        clock.prepare (sampleRate);

        for (auto& voice : voices)
        {
            voice.bell.prepare (spec);
            voice.lfo.prepare (sampleRate);
            voice.frequencyHz.reset (sampleRate, smoothingSeconds);
            voice.gainDb.reset (sampleRate, smoothingSeconds);
            voice.q.reset (sampleRate, smoothingSeconds);
            voice.move.reset (sampleRate, smoothingSeconds);
        }

        wet.reset (sampleRate, onOffFadeSeconds);
        reset();
    }

    // Jumps every smoothed setting straight to its target and clears the filters.
    void TwistStage::reset() noexcept
    {
        for (auto& voice : voices)
        {
            voice.bell.reset();
            voice.lfo.reset();
            voice.frequencyHz.setCurrentAndTargetValue (voice.frequencyHz.getTargetValue());
            voice.gainDb.setCurrentAndTargetValue (voice.gainDb.getTargetValue());
            voice.q.setCurrentAndTargetValue (voice.q.getTargetValue());
            voice.move.setCurrentAndTargetValue (voice.move.getTargetValue());
        }

        wet.setCurrentAndTargetValue (wet.getTargetValue());
        restartFreePhases();
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
        voice.gainDb.setTargetValue (settings.gainDb);
        voice.q.setTargetValue (juce::jlimit (0.1f, 10.0f, settings.q));
        voice.move.setTargetValue (juce::jlimit (-1.0f, 1.0f, settings.move));
        voice.lfo.setShape (settings.shape);
        voice.syncSpeedIndex = settings.syncSpeedIndex;
        voice.freeSpeedHz = juce::jmax (0.0f, settings.freeSpeedHz);
        voice.startCycles = settings.startCycles;
    }

    void TwistStage::setTransport (const TwistTransport& transport) noexcept
    {
        clock.setTransport (transport);

        // "Sync off" runs freely from when playback starts.
        if (clock.takePlaybackStarted())
            restartFreePhases();
    }

    void TwistStage::restartFreePhases() noexcept
    {
        for (auto& voice : voices)
            voice.freePhase = 0.0;
    }

    bool TwistStage::isFullyOff() const noexcept
    {
        return ! wet.isSmoothing() && wet.getTargetValue() <= 0.0f;
    }

    // Keeps time moving while off, so switching on lands in the right place.
    void TwistStage::skipWhileOff (size_t numSamples) noexcept
    {
        clock.advance (static_cast<int> (numSamples));

        for (auto& voice : voices)
            voice.freePhase += voice.freeSpeedHz * static_cast<double> (numSamples) / sampleRate;

        for (auto& voice : voices)
            voice.currentFrequencyHz = voice.frequencyHz.getTargetValue();

        if (bellsNeedClearing)
        {
            for (auto& voice : voices)
            {
                voice.bell.reset();
                voice.lfo.reset();
            }

            bellsNeedClearing = false;
        }
    }

    double TwistStage::nextCyclePosition (Voice& voice) noexcept
    {
        if (synced)
            return clock.getBeats() / clock.getCycleLengthInBeats (voice.syncSpeedIndex) + voice.startCycles;

        const auto position = voice.freePhase + voice.startCycles;
        voice.freePhase += voice.freeSpeedHz / sampleRate;
        return position;
    }

    void TwistStage::updateBellShape (Voice& voice) noexcept
    {
        const auto lfoValue = voice.lfo.next (nextCyclePosition (voice));
        const auto sweepOctaves = voice.move.getNextValue() * maxSweepOctaves * lfoValue;
        const auto frequency = voice.frequencyHz.getNextValue() * std::exp2 (sweepOctaves);
        voice.currentFrequencyHz = juce::jlimit (20.0f, voice.bell.getMaxFrequencyHz(), frequency);

        voice.bell.setShape (frequency, voice.gainDb.getNextValue(), voice.q.getNextValue());
    }

    void TwistStage::processFrame (juce::dsp::AudioBlock<float>& block, size_t index, float wetAmount) noexcept
    {
        for (auto& voice : voices)
            updateBellShape (voice);

        for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
        {
            auto* samples = block.getChannelPointer (ch);
            const auto dry = samples[index];
            auto twisted = dry;

            for (auto& voice : voices)
                twisted = voice.bell.processSample (ch, twisted);

            samples[index] = dry + wetAmount * (twisted - dry);
        }

        clock.advance();
    }

    float TwistStage::getCurrentFrequencyHz (int bellIndex) const noexcept
    {
        return voices[static_cast<size_t> (juce::jlimit (0, numBells - 1, bellIndex))].currentFrequencyHz;
    }

    void TwistStage::process (juce::dsp::AudioBlock<float>& block) noexcept
    {
        const auto numSamples = block.getNumSamples();

        if (isFullyOff())
        {
            skipWhileOff (numSamples);
            return;
        }

        bellsNeedClearing = true;

        for (size_t i = 0; i < numSamples; ++i)
            processFrame (block, i, wet.getNextValue());
    }
}
