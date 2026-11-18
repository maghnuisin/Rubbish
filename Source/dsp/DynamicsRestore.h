#pragma once

#include <juce_dsp/juce_dsp.h>

namespace rubbish
{
    /** Dynamics Restore: distortion flattens the punch out of a sound. This puts
        the clean input's moment-to-moment volume shape back onto the distorted signal.

        It compares how far each signal currently rises above or sinks below its
        own recent average and corrects the difference, so it restores the shape (the hits and
        the gaps) without changing the overall level. */
    class DynamicsRestore
    {
    public:
        void prepare (const juce::dsp::ProcessSpec& spec, int processingLatencySamples);
        void reset();

        void setAmount (float newAmount) noexcept; // 0..1

        /** inputLoudness: the shared tracker's envelope for this block (not yet delayed). */
        void process (juce::dsp::AudioBlock<float>& processed, const float* inputLoudness) noexcept;

    private:
        // Both in decibels. Averaging in decibels means the correction doesn't shift
        // the average it is measured against, so 100% reproduces the input's shape.
        struct Levels
        {
            float nowDb = -80.0f;
            float averageDb = -80.0f;
        };

        void updateLevels (float inputNow, float processedNow) noexcept;
        float restoringGain (float amountNow) const noexcept;
        void updateLevel (Levels& levels, float loudness) const noexcept;

        static constexpr double averageSeconds = 0.3;
        static constexpr double smoothingSeconds = 0.05;
        static constexpr float maxCutDb = -24.0f;
        static constexpr float maxBoostDb = 18.0f;

        juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::None> inputDelay;
        juce::dsp::BallisticsFilter<float> processedFollower;
        float averageCoefficient = 0.0f;

        Levels input, processed;
        juce::SmoothedValue<float> amount { 0.0f };
    };
}
