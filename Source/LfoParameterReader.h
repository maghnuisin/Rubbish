#pragma once

#include "dsp/LfoBank.h"
#include "dsp/ModulationMatrix.h"

#include <juce_audio_processors/juce_audio_processors.h>

#include <array>

namespace rubbish
{
    /** Reads the LFO section's knobs and every ring amount on the audio thread
        (lock-free) and turns them into the units LfoBank and ModulationMatrix use. */
    class LfoParameterReader
    {
    public:
        explicit LfoParameterReader (juce::AudioProcessorValueTreeState& state);

        LfoSettings readLfo (int lfoIndex) const noexcept;

        /** -1..+1 (the ring's -100%..+100%). */
        float readAmount (int lfoIndex, int targetIndex) const noexcept;

    private:
        struct LfoParameters
        {
            std::atomic<float>* shape;
            std::atomic<float>* curve;
            std::atomic<float>* sync;
            std::atomic<float>* speedSync;
            std::atomic<float>* speedFree;
            std::atomic<float>* start;
            std::atomic<float>* smooth;
            std::atomic<float>* jitter;
            std::atomic<float>* offset;
            std::atomic<float>* unipolar;
        };

        using AmountRow = std::array<std::atomic<float>*, ModulationMatrix::numTargets>;

        static LfoParameters findLfoParameters (juce::AudioProcessorValueTreeState& state, int lfoIndex);
        static AmountRow findAmounts (juce::AudioProcessorValueTreeState& state, int lfoIndex);

        std::array<LfoParameters, LfoBank::numLfos> lfos;
        std::array<AmountRow, LfoBank::numLfos> amounts;
    };
}
