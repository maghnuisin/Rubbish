#pragma once

#include "TwistBell.h"

#include <array>

namespace rubbish
{
    /** One Twist bell's home settings, already converted from the knobs. */
    struct TwistBellSettings
    {
        float frequencyHz = 1000.0f;
        float gainDb = 0.0f;
        float qPercent = 37.5f; // the Q knob's 0-100%
    };

    /** How the LFO rings are moving one bell, sample by sample, in "knob travels"
        (see ModulationMatrix). nullptr = that knob isn't moving. */
    struct TwistBellMovement
    {
        const float* frequency = nullptr; // 1 = +3 octaves
        const float* gain = nullptr;      // 1 = the whole Gain range
        const float* q = nullptr;         // 1 = the whole Q range
    };

    /** Twist: three EQ bells in a row, which the LFO section moves around.
        Switching Twist on or off fades it in or out, and while fully off it costs nothing. */
    class TwistStage
    {
    public:
        static constexpr int numBells = 3;
        using Movement = std::array<TwistBellMovement, numBells>;

        void prepare (const juce::dsp::ProcessSpec& spec);
        void reset() noexcept;

        void setEnabled (bool shouldBeEnabled) noexcept;
        void setBell (int bellIndex, const TwistBellSettings& settings) noexcept;

        void process (juce::dsp::AudioBlock<float>& block, const Movement& movement) noexcept;

        /** Where a bell is right now, after the LFOs have moved it (its home position while Twist is off). */
        float getCurrentFrequencyHz (int bellIndex) const noexcept;

    private:
        struct Voice
        {
            TwistBell bell;
            juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> frequencyHz { 1000.0f };
            juce::SmoothedValue<float> gainDb { 0.0f };
            juce::SmoothedValue<float> qPercent { 37.5f };
            float currentFrequencyHz = 1000.0f;
        };

        bool isFullyOff() const noexcept;
        void skipWhileOff() noexcept;
        void updateBellShape (Voice& voice, const TwistBellMovement& movement, size_t index) noexcept;
        void processFrame (juce::dsp::AudioBlock<float>& block, size_t index, float wetAmount, const Movement& movement) noexcept;

        static constexpr double smoothingSeconds = 0.05;
        static constexpr double onOffFadeSeconds = 0.02;
        static constexpr float gainRangeDb = 48.0f;  // -24..+24 dB
        static constexpr float qRangePercent = 100.0f;

        std::array<Voice, numBells> voices;
        juce::SmoothedValue<float> wet { 0.0f };
        bool bellsNeedClearing = false;
    };
}
