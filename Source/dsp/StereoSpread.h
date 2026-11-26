#pragma once

#include "CharacterShaper.h"

#include <juce_dsp/juce_dsp.h>

namespace rubbish
{
    /** Spread: distorts the stereo sides (the difference between left and right)
        harder than the centre, for extra width.

        Only the side part of the signal is changed, and side content cancels
        exactly when left and right are summed to mono, so the mono sum is
        identical at every Spread setting. A mono source has no sides, so it
        is left alone. Runs inside the oversampled distortion. */
    class StereoSpread
    {
    public:
        void prepare (double oversampledRate, size_t maxOversampledBlockSize);
        void reset();

        void setAmount (float newAmount) noexcept; // 0..1
        void setCharacter (Character newCharacter) noexcept;
        void setDrive (const DriveState& newDrive) noexcept;

        /** Call with the driven signal, before the main Character shaping. */
        void captureSides (const juce::dsp::AudioBlock<float>& driven) noexcept;

        /** Call after the main shaping to swap in the harder-driven sides. */
        void apply (juce::dsp::AudioBlock<float>& shaped) noexcept;

    private:
        static bool isStereo (const juce::dsp::AudioBlock<float>& block) noexcept;

        static constexpr float sideDriveBoost = 4.0f; // +12 dB more drive on the sides
        static constexpr double smoothingSeconds = 0.05;

        CharacterShaper sideShaper;
        juce::AudioBuffer<float> sides;
        size_t capturedSamples = 0;
        juce::SmoothedValue<float> amount { 0.0f };
    };
}
