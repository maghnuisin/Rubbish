#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

#include <array>
#include <vector>

namespace rubbish
{
    class LfoBank;

    /** The rings: how much each LFO moves each knob. For every knob it adds up
        (ring amount x LFO value) from all three LFOs, sample by sample.

        The result is in "knob travels" (-1..+1 or beyond, before clamping): 1 means
        one whole knob range (or, for a bell's Freq, the full ±3-octave sweep). */
    class ModulationMatrix
    {
    public:
        static constexpr int numLfos = 3;
        static constexpr int numTargets = 20;

        void prepare (double sampleRate, int maximumBlockSize);
        void reset() noexcept;

        /** amount is -1..+1 (the ring's -100%..+100%). */
        void setAmount (int lfoIndex, int targetIndex, float amount) noexcept;

        void process (const LfoBank& lfos, int numSamples) noexcept;

        /** The movement for one knob from the last process(), or nullptr if nothing moves it. */
        const float* getModulation (int targetIndex) const noexcept;

    private:
        using Amount = juce::SmoothedValue<float>;

        bool isConnected (int targetIndex) const noexcept;
        void mixTarget (int targetIndex, const LfoBank& lfos, int numSamples) noexcept;

        static constexpr double smoothingSeconds = 0.05;

        std::array<std::array<Amount, numTargets>, numLfos> amounts;
        std::array<std::vector<float>, numTargets> movement;
        std::array<bool, numTargets> moving {};
    };
}
