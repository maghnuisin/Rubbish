#pragma once

#include <juce_dsp/juce_dsp.h>

namespace rubbish
{
    /** Clean Low: splits off everything below the cutoff so it skips the
        distortion and filters, then adds it back afterwards, delayed to stay in
        line with the oversampled distortion. At the 20 Hz minimum there is no
        split at all: the full signal goes through untouched by the crossover. */
    class CleanLowSplitter
    {
    public:
        void prepare (const juce::dsp::ProcessSpec& spec, int processingLatencySamples);
        void reset();

        void setCutoffHz (float newCutoffHz) noexcept;

        /** Leaves the high band in the block and keeps the low band aside. */
        void split (juce::dsp::AudioBlock<float>& block) noexcept;

        /** Adds the kept low band back into the processed high band. */
        void recombine (juce::dsp::AudioBlock<float>& block) noexcept;

    private:
        void advanceCrossover() noexcept;

        static constexpr float noSplitHz = 20.0f;
        static constexpr double smoothingSeconds = 0.05;

        juce::dsp::LinkwitzRileyFilter<float> crossover;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> cutoffHz { noSplitHz };
        juce::SmoothedValue<float> splitAmount { 0.0f };

        juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::None> lowBandDelay;
        juce::AudioBuffer<float> lowBand;
    };
}
