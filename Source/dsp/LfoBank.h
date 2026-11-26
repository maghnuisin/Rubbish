#pragma once

#include "Lfo.h"
#include "TwistClock.h"

#include <array>
#include <vector>

namespace rubbish
{
    /** One LFO's full settings, already converted from the knobs. */
    struct LfoSettings
    {
        LfoShapeSettings shape;
        bool synced = true;
        int syncSpeedIndex = 12;
        float freeSpeedHz = 0.5f;
        float startCycles = 0.0f; // 0 to 1 (Start knob / 360 degrees)
    };

    /** The three LFOs and their shared sense of musical time. Each audio chunk it
        writes every LFO's value for every sample, ready for the rings to use.
        The LFOs keep running whether or not Twist is on, since they can move any knob. */
    class LfoBank
    {
    public:
        static constexpr int numLfos = 3;

        void prepare (double sampleRate, int maximumBlockSize);
        void reset() noexcept;

        void setLfo (int lfoIndex, const LfoSettings& settings) noexcept;

        /** Call once per DAW audio block, before render(). */
        void setTransport (const TwistTransport& transport) noexcept;

        /** Works out the next numSamples values of every LFO and moves time on. */
        void render (int numSamples) noexcept;

        /** The values from the last render(), one per sample. */
        const float* getValues (int lfoIndex) const noexcept;

    private:
        struct Voice
        {
            explicit Voice (int seed) : lfo (seed) {}

            Lfo lfo;
            LfoSettings settings;
            double freePhase = 0.0;
            std::vector<float> values;
        };

        double nextCyclePosition (Voice& voice) noexcept;
        double cycleSeconds (const LfoSettings& settings) const noexcept;
        void restartFreePhases() noexcept;

        std::array<Voice, numLfos> voices { Voice { 1 }, Voice { 2 }, Voice { 3 } };
        TwistClock clock;
        double sampleRate = 44100.0;
        double bpm = 120.0;
    };
}
