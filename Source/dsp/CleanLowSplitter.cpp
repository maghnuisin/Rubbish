#include "CleanLowSplitter.h"

namespace rubbish
{
    void CleanLowSplitter::prepare (const juce::dsp::ProcessSpec& spec, int processingLatencySamples)
    {
        jassert (processingLatencySamples >= 0);

        crossover.prepare (spec);
        crossover.setCutoffFrequency (cutoffHz.getTargetValue());

        cutoffHz.reset (spec.sampleRate, smoothingSeconds);
        splitAmount.reset (spec.sampleRate, smoothingSeconds);

        lowBandDelay.prepare (spec);
        lowBandDelay.setMaximumDelayInSamples (processingLatencySamples + 1);
        lowBandDelay.setDelay (static_cast<float> (processingLatencySamples));

        lowBand.setSize (static_cast<int> (spec.numChannels), static_cast<int> (spec.maximumBlockSize));
        reset();
    }

    void CleanLowSplitter::reset()
    {
        cutoffHz.setCurrentAndTargetValue (cutoffHz.getTargetValue());
        splitAmount.setCurrentAndTargetValue (splitAmount.getTargetValue());
        crossover.setCutoffFrequency (cutoffHz.getTargetValue());
        crossover.reset();
        lowBandDelay.reset();
        lowBand.clear();
    }

    void CleanLowSplitter::setCutoffHz (float newCutoffHz) noexcept
    {
        const auto isSplitting = newCutoffHz > noSplitHz + 0.5f;
        splitAmount.setTargetValue (isSplitting ? 1.0f : 0.0f);
        cutoffHz.setTargetValue (juce::jmax (noSplitHz, newCutoffHz));
    }

    void CleanLowSplitter::advanceCrossover() noexcept
    {
        if (cutoffHz.isSmoothing())
            crossover.setCutoffFrequency (cutoffHz.getNextValue());
    }

    // Blending between "split" and "no split" means sliding away from 20 Hz
    // fades the crossover in smoothly instead of switching it on with a jump.
    void CleanLowSplitter::split (juce::dsp::AudioBlock<float>& block) noexcept
    {
        jassert (block.getNumChannels() <= static_cast<size_t> (lowBand.getNumChannels()));
        jassert (block.getNumSamples() <= static_cast<size_t> (lowBand.getNumSamples()));

        for (size_t i = 0; i < block.getNumSamples(); ++i)
        {
            advanceCrossover();
            const auto amount = splitAmount.getNextValue();

            for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
            {
                auto& sample = block.getChannelPointer (ch)[i];
                float low = 0.0f, high = 0.0f;
                crossover.processSample (static_cast<int> (ch), sample, low, high);

                lowBand.setSample (static_cast<int> (ch), static_cast<int> (i), amount * low);
                sample = amount * high + (1.0f - amount) * sample;
            }
        }
    }

    void CleanLowSplitter::recombine (juce::dsp::AudioBlock<float>& block) noexcept
    {
        for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
        {
            const auto channel = static_cast<int> (ch);
            auto* data = block.getChannelPointer (ch);

            for (size_t i = 0; i < block.getNumSamples(); ++i)
            {
                lowBandDelay.pushSample (channel, lowBand.getSample (channel, static_cast<int> (i)));
                data[i] += lowBandDelay.popSample (channel);
            }
        }
    }
}
