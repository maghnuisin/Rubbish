#pragma once

#include <optional>

namespace rubbish
{
    /** What the DAW says about playback, read once per audio block. */
    struct TwistTransport
    {
        double bpm = 120.0;
        std::optional<double> songPositionBeats; // in quarter notes, while playing
        bool isPlaying = false;
        double quarterNotesPerBar = 4.0;
    };

    /** Keeps Twist's sense of musical time. While the DAW plays, it locks to the song
        position, so the same bar moves the same way every time. While stopped, it keeps
        counting at the DAW tempo so the bells still move when you audition sounds. */
    class TwistClock
    {
    public:
        static constexpr int numSyncSpeeds = 16;

        void prepare (double sampleRate);
        void reset() noexcept;

        /** Call once per audio block, before advancing through its samples. */
        void setTransport (const TwistTransport& transport) noexcept;

        /** True once, on the first block after playback starts. */
        bool takePlaybackStarted() noexcept;

        void advance (int numSamples = 1) noexcept { beats += beatsPerSample * numSamples; }
        double getBeats() const noexcept { return beats; }

        /** Length of one LFO cycle, in quarter notes, for a Speed (Sync on) choice. */
        double getCycleLengthInBeats (int syncSpeedIndex) const noexcept;

    private:
        double sampleRate = 44100.0;
        double beats = 0.0;
        double beatsPerSample = 0.0;
        double quarterNotesPerBar = 4.0;
        bool wasPlaying = false;
        bool playbackStarted = false;
    };
}
