#include "TwistClock.h"

#include <juce_core/juce_core.h>

namespace rubbish
{
    void TwistClock::prepare (double newSampleRate)
    {
        jassert (newSampleRate > 0.0);
        sampleRate = newSampleRate;
        reset();
        setTransport ({});
    }

    void TwistClock::reset() noexcept
    {
        beats = 0.0;
        wasPlaying = false;
        playbackStarted = false;
    }

    void TwistClock::setTransport (const TwistTransport& transport) noexcept
    {
        const auto bpm = transport.bpm > 0.0 ? transport.bpm : 120.0;
        beatsPerSample = bpm / (60.0 * sampleRate);
        quarterNotesPerBar = transport.quarterNotesPerBar > 0.0 ? transport.quarterNotesPerBar : 4.0;

        if (transport.isPlaying && transport.songPositionBeats.has_value())
            beats = *transport.songPositionBeats;

        if (transport.isPlaying && ! wasPlaying)
            playbackStarted = true;

        wasPlaying = transport.isPlaying;
    }

    bool TwistClock::takePlaybackStarted() noexcept
    {
        const auto started = playbackStarted;
        playbackStarted = false;
        return started;
    }

    double TwistClock::getCycleLengthInBeats (int syncSpeedIndex) const noexcept
    {
        // Note lengths in quarter notes, each as triplet, straight, dotted:
        // 1/16, 1/8, 1/4 and 1/2. Then 1, 2, 4 and 8 bars.
        constexpr double triplet = 2.0 / 3.0;
        constexpr double dotted = 1.5;
        constexpr double noteLengths[] { 0.25 * triplet, 0.25, 0.25 * dotted,
                                         0.5 * triplet,  0.5,  0.5 * dotted,
                                         1.0 * triplet,  1.0,  1.0 * dotted,
                                         2.0 * triplet,  2.0,  2.0 * dotted };
        constexpr int barCounts[] { 1, 2, 4, 8 };
        constexpr int numNoteLengths = 12;
        static_assert (numNoteLengths + 4 == numSyncSpeeds);

        const auto index = juce::jlimit (0, numSyncSpeeds - 1, syncSpeedIndex);

        if (index < numNoteLengths)
            return noteLengths[index];

        return barCounts[index - numNoteLengths] * quarterNotesPerBar;
    }
}
