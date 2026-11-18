#pragma once

#include <juce_dsp/juce_dsp.h>

namespace rubbish
{
    /** Works out the gain that makes one signal (the target) as loud on average as
        another (the reference). It listens to both over roughly a second and adjusts
        slowly. While the reference is near-silent it holds its last gain, so it
        never pumps up noise or decaying tails. Used by Auto Gain and Delta. */
    class LoudnessMatcher
    {
    public:
        enum class Listen
        {
            allChannels, // total loudness of every channel
            centre       // only the mono (L+R) part, so stereo width doesn't sway it
        };

        LoudnessMatcher (Listen whatToListenTo, float minimumGainDb, float maximumGainDb);

        void prepare (double sampleRate);
        void reset();

        /** Feed the reference signal (here, always the untouched input). */
        void measureReference (const juce::dsp::AudioBlock<float>& reference) noexcept;

        /** Measures the target and writes one matching gain per sample into gainsOut.
            The target block itself is not changed. */
        void computeGains (const juce::dsp::AudioBlock<float>& target, float* gainsOut) noexcept;

    private:
        float meanSquareAt (const juce::dsp::AudioBlock<float>& block, size_t index) const noexcept;
        static float meanSquareAcrossChannels (const juce::dsp::AudioBlock<float>& block, size_t index) noexcept;
        static float centreSquared (const juce::dsp::AudioBlock<float>& block, size_t index) noexcept;
        float nextGain (float targetSampleMeanSquare) noexcept;

        static constexpr double averagingSeconds = 0.4;
        static constexpr double gainSmoothingSeconds = 0.05;

        const Listen listen;
        const float minimumGain;
        const float maximumGain;

        float averagingCoefficient = 0.0f;
        float gainSmoothingCoefficient = 0.0f;

        float referenceMeanSquare = 0.0f;
        float targetMeanSquare = 0.0f;
        float heldGain = 1.0f;
        float smoothedGain = 1.0f;
    };
}
