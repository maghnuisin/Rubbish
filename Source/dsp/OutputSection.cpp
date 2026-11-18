#include "OutputSection.h"

namespace rubbish
{
    void OutputSection::prepare (const juce::dsp::ProcessSpec& spec, int processingLatencySamples)
    {
        jassert (processingLatencySamples >= 0);

        autoGainMatcher.prepare (spec.sampleRate);
        deltaMatcher.prepare (spec.sampleRate);

        for (auto* smoother : { &autoGainAmount, &mix, &deltaAmount })
            smoother->reset (spec.sampleRate, rampSeconds);

        dryDelay.prepare (spec);
        dryDelay.setMaximumDelayInSamples (processingLatencySamples + 1);
        dryDelay.setDelay (static_cast<float> (processingLatencySamples));

        const auto channels = static_cast<int> (spec.numChannels);
        const auto samples = static_cast<int> (spec.maximumBlockSize);
        alignedDry.setSize (channels, samples);
        delta.setSize (channels, samples);
        autoGains.assign (spec.maximumBlockSize, 1.0f);
        deltaGains.assign (spec.maximumBlockSize, 1.0f);

        outputGain.prepare (spec);
        outputGain.setRampDurationSeconds (rampSeconds);
        limiter.prepare (spec.sampleRate);
        reset();
    }

    void OutputSection::reset()
    {
        autoGainMatcher.reset();
        deltaMatcher.reset();

        for (auto* smoother : { &autoGainAmount, &mix, &deltaAmount })
            smoother->setCurrentAndTargetValue (smoother->getTargetValue());

        dryDelay.reset();
        alignedDry.clear();
        delta.clear();
        outputGain.reset();
        limiter.reset();
    }

    void OutputSection::setAutoGainEnabled (bool shouldBeEnabled) noexcept
    {
        autoGainAmount.setTargetValue (shouldBeEnabled ? 1.0f : 0.0f);
    }

    void OutputSection::setMix (float wetProportion) noexcept
    {
        mix.setTargetValue (juce::jlimit (0.0f, 1.0f, wetProportion));
    }

    void OutputSection::setDeltaEnabled (bool shouldBeEnabled) noexcept
    {
        deltaAmount.setTargetValue (shouldBeEnabled ? 1.0f : 0.0f);
    }

    void OutputSection::setOutputDecibels (float newOutputDb) noexcept
    {
        outputGain.setGainDecibels (newOutputDb);
    }

    juce::dsp::AudioBlock<float> OutputSection::getDryBlock (size_t numChannels, size_t numSamples) noexcept
    {
        return juce::dsp::AudioBlock<float> (alignedDry).getSubsetChannelBlock (0, numChannels).getSubBlock (0, numSamples);
    }

    juce::dsp::AudioBlock<float> OutputSection::getDeltaBlock (size_t numChannels, size_t numSamples) noexcept
    {
        return juce::dsp::AudioBlock<float> (delta).getSubsetChannelBlock (0, numChannels).getSubBlock (0, numSamples);
    }

    void OutputSection::captureDry (const juce::dsp::AudioBlock<float>& input) noexcept
    {
        jassert (input.getNumSamples() <= autoGains.size());
        jassert (input.getNumChannels() <= static_cast<size_t> (alignedDry.getNumChannels()));

        autoGainMatcher.measureReference (input);
        deltaMatcher.measureReference (input);
        delayDry (input);
    }

    void OutputSection::delayDry (const juce::dsp::AudioBlock<float>& input) noexcept
    {
        auto dry = getDryBlock (input.getNumChannels(), input.getNumSamples());

        for (size_t ch = 0; ch < input.getNumChannels(); ++ch)
        {
            const auto channel = static_cast<int> (ch);
            const auto* source = input.getChannelPointer (ch);
            auto* destination = dry.getChannelPointer (ch);

            for (size_t i = 0; i < input.getNumSamples(); ++i)
            {
                dryDelay.pushSample (channel, source[i]);
                destination[i] = dryDelay.popSample (channel);
            }
        }
    }

    void OutputSection::process (juce::dsp::AudioBlock<float>& wet) noexcept
    {
        autoGainMatcher.computeGains (wet, autoGains.data());
        buildDelta (wet);
        blendIntoOutput (wet);

        outputGain.process (juce::dsp::ProcessContextReplacing<float> (wet));
        limiter.process (wet);
    }

    // Delta compares the level-matched processed signal with the original, so it
    // holds what the distortion changed rather than just a volume difference.
    // It ignores the Auto Gain switch and Mix, then is lifted to be clearly audible.
    void OutputSection::buildDelta (const juce::dsp::AudioBlock<float>& wet) noexcept
    {
        const auto dry = getDryBlock (wet.getNumChannels(), wet.getNumSamples());
        auto deltaBlock = getDeltaBlock (wet.getNumChannels(), wet.getNumSamples());

        for (size_t ch = 0; ch < wet.getNumChannels(); ++ch)
            for (size_t i = 0; i < wet.getNumSamples(); ++i)
                deltaBlock.getChannelPointer (ch)[i] = wet.getChannelPointer (ch)[i] * autoGains[i]
                                                     - dry.getChannelPointer (ch)[i];

        deltaMatcher.computeGains (deltaBlock, deltaGains.data());

        for (size_t ch = 0; ch < wet.getNumChannels(); ++ch)
            juce::FloatVectorOperations::multiply (deltaBlock.getChannelPointer (ch), deltaGains.data(),
                                                   static_cast<int> (wet.getNumSamples()));
    }

    void OutputSection::blendIntoOutput (juce::dsp::AudioBlock<float>& wet) noexcept
    {
        const auto dry = getDryBlock (wet.getNumChannels(), wet.getNumSamples());
        const auto deltaBlock = getDeltaBlock (wet.getNumChannels(), wet.getNumSamples());

        for (size_t i = 0; i < wet.getNumSamples(); ++i)
        {
            const auto wetGain = 1.0f + autoGainAmount.getNextValue() * (autoGains[i] - 1.0f);
            const auto wetMix = mix.getNextValue();
            const auto deltaMix = deltaAmount.getNextValue();

            for (size_t ch = 0; ch < wet.getNumChannels(); ++ch)
            {
                auto& sample = wet.getChannelPointer (ch)[i];
                const auto normal = dry.getChannelPointer (ch)[i] * (1.0f - wetMix) + sample * wetGain * wetMix;
                sample = normal + deltaMix * (deltaBlock.getChannelPointer (ch)[i] - normal);
            }
        }
    }
}
