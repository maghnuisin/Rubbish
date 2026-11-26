#include "LfoBank.h"

namespace rubbish
{
    void LfoBank::prepare (double newSampleRate, int maximumBlockSize)
    {
        jassert (newSampleRate > 0.0 && maximumBlockSize > 0);
        sampleRate = newSampleRate;
        clock.prepare (sampleRate);

        for (auto& voice : voices)
        {
            voice.lfo.prepare (sampleRate);
            voice.values.assign (static_cast<size_t> (maximumBlockSize), 0.0f);
        }

        reset();
    }

    void LfoBank::reset() noexcept
    {
        for (auto& voice : voices)
            voice.lfo.reset();

        restartFreePhases();
    }

    void LfoBank::setLfo (int lfoIndex, const LfoSettings& settings) noexcept
    {
        jassert (lfoIndex >= 0 && lfoIndex < numLfos);
        auto& voice = voices[static_cast<size_t> (juce::jlimit (0, numLfos - 1, lfoIndex))];
        voice.settings = settings;
        voice.settings.freeSpeedHz = juce::jmax (0.0f, settings.freeSpeedHz);
        voice.lfo.setShape (settings.shape, cycleSeconds (voice.settings));
    }

    void LfoBank::setTransport (const TwistTransport& transport) noexcept
    {
        bpm = transport.bpm > 0.0 ? transport.bpm : 120.0;
        clock.setTransport (transport);

        // "Sync off" runs freely from when playback starts.
        if (clock.takePlaybackStarted())
            restartFreePhases();
    }

    void LfoBank::restartFreePhases() noexcept
    {
        for (auto& voice : voices)
            voice.freePhase = 0.0;
    }

    double LfoBank::cycleSeconds (const LfoSettings& settings) const noexcept
    {
        if (settings.synced)
            return clock.getCycleLengthInBeats (settings.syncSpeedIndex) * 60.0 / bpm;

        return settings.freeSpeedHz > 0.0f ? 1.0 / settings.freeSpeedHz : 0.0;
    }

    double LfoBank::nextCyclePosition (Voice& voice) noexcept
    {
        const auto& s = voice.settings;

        if (s.synced)
            return clock.getBeats() / clock.getCycleLengthInBeats (s.syncSpeedIndex) + s.startCycles;

        const auto position = voice.freePhase + s.startCycles;
        voice.freePhase += s.freeSpeedHz / sampleRate;
        return position;
    }

    void LfoBank::render (int numSamples) noexcept
    {
        jassert (numSamples <= static_cast<int> (voices[0].values.size()));
        const auto count = juce::jmin (numSamples, static_cast<int> (voices[0].values.size()));

        for (int i = 0; i < count; ++i)
        {
            for (auto& voice : voices)
                voice.values[static_cast<size_t> (i)] = voice.lfo.next (nextCyclePosition (voice));

            clock.advance();
        }
    }

    const float* LfoBank::getValues (int lfoIndex) const noexcept
    {
        return voices[static_cast<size_t> (juce::jlimit (0, numLfos - 1, lfoIndex))].values.data();
    }
}
