#pragma once

#include "TwistBell.h"
#include "TwistClock.h"
#include "TwistLfo.h"

#include <array>

namespace rubbish
{
    /** One Twist bell's settings, already converted from the knobs. */
    struct TwistBellSettings
    {
        float frequencyHz = 1000.0f;
        float gainDb = 0.0f;
        float q = 0.5f;
        TwistShape shape = TwistShape::smooth;
        int syncSpeedIndex = 12;
        float freeSpeedHz = 0.5f;
        float move = 0.0f;        // -1 to +1; 0 = the bell stays still
        float startCycles = 0.0f; // 0 to 1 (Start knob / 360 degrees)
    };

    /** Twist: three EQ bells in a row, each swept up and down in frequency by its own LFO.
        Switching Twist on or off fades it in or out, and while fully off it costs nothing. */
    class TwistStage
    {
    public:
        static constexpr int numBells = 3;

        void prepare (const juce::dsp::ProcessSpec& spec);
        void reset() noexcept;

        void setEnabled (bool shouldBeEnabled) noexcept;
        void setSynced (bool shouldFollowTempo) noexcept { synced = shouldFollowTempo; }
        void setBell (int bellIndex, const TwistBellSettings& settings) noexcept;

        /** Call once per DAW audio block, before process(). */
        void setTransport (const TwistTransport& transport) noexcept;

        void process (juce::dsp::AudioBlock<float>& block) noexcept;

        /** Where a bell is right now, after its LFO has moved it (its home position while Twist is off). */
        float getCurrentFrequencyHz (int bellIndex) const noexcept;

    private:
        struct Voice
        {
            explicit Voice (int seed) : lfo (seed) {}

            TwistBell bell;
            TwistLfo lfo;
            juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> frequencyHz { 1000.0f };
            juce::SmoothedValue<float> gainDb { 0.0f };
            juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> q { 0.5f };
            juce::SmoothedValue<float> move { 0.0f };
            int syncSpeedIndex = 12;
            float freeSpeedHz = 0.5f;
            float startCycles = 0.0f;
            double freePhase = 0.0;
            float currentFrequencyHz = 1000.0f;
        };

        bool isFullyOff() const noexcept;
        void skipWhileOff (size_t numSamples) noexcept;
        void restartFreePhases() noexcept;
        double nextCyclePosition (Voice& voice) noexcept;
        void updateBellShape (Voice& voice) noexcept;
        void processFrame (juce::dsp::AudioBlock<float>& block, size_t index, float wetAmount) noexcept;

        static constexpr double smoothingSeconds = 0.05;
        static constexpr double onOffFadeSeconds = 0.02;
        static constexpr float maxSweepOctaves = 3.0f; // Move at 100% sweeps 3 octaves each way

        std::array<Voice, numBells> voices { Voice { 1 }, Voice { 2 }, Voice { 3 } };
        TwistClock clock;
        juce::SmoothedValue<float> wet { 0.0f };
        double sampleRate = 44100.0;
        bool synced = true;
        bool bellsNeedClearing = false;
    };
}
