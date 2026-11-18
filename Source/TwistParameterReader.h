#pragma once

#include "dsp/TwistStage.h"

#include <juce_audio_processors/juce_audio_processors.h>

#include <array>

namespace rubbish
{
    /** Reads the Twist knobs on the audio thread (lock-free) and turns them into
        the units TwistStage works in. */
    class TwistParameterReader
    {
    public:
        explicit TwistParameterReader (juce::AudioProcessorValueTreeState& state);

        bool isEnabled() const noexcept;
        bool isSynced() const noexcept;
        TwistBellSettings readBell (int bellIndex) const noexcept;

    private:
        struct BellParameters
        {
            std::atomic<float>* frequency;
            std::atomic<float>* gain;
            std::atomic<float>* q;
            std::atomic<float>* shape;
            std::atomic<float>* speedSync;
            std::atomic<float>* speedFree;
            std::atomic<float>* move;
            std::atomic<float>* start;
        };

        static BellParameters findBellParameters (juce::AudioProcessorValueTreeState& state, int bellIndex);

        std::atomic<float>& enabled;
        std::atomic<float>& synced;
        std::array<BellParameters, TwistStage::numBells> bells;
    };
}
