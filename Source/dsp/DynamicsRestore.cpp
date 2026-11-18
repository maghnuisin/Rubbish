#include "DynamicsRestore.h"
#include "BlockHelpers.h"
#include "LoudnessTracker.h"

#include <cmath>

namespace rubbish
{
    namespace
    {
        constexpr float silenceDb = -80.0f; // keeps the comparison sane in quiet passages
    }

    void DynamicsRestore::prepare (const juce::dsp::ProcessSpec& spec, int processingLatencySamples)
    {
        jassert (processingLatencySamples >= 0);

        // The input loudness is delayed to line up with the oversampled, delayed signal.
        inputDelay.prepare ({ spec.sampleRate, spec.maximumBlockSize, 1 });
        inputDelay.setMaximumDelayInSamples (processingLatencySamples + 1);
        inputDelay.setDelay (static_cast<float> (processingLatencySamples));

        LoudnessTracker::configure (processedFollower, spec.sampleRate);
        averageCoefficient = static_cast<float> (1.0 - std::exp (-1.0 / (averageSeconds * spec.sampleRate)));
        amount.reset (spec.sampleRate, smoothingSeconds);
        reset();
    }

    void DynamicsRestore::reset()
    {
        inputDelay.reset();
        processedFollower.reset();
        input = {};
        processed = {};
        amount.setCurrentAndTargetValue (amount.getTargetValue());
    }

    void DynamicsRestore::setAmount (float newAmount) noexcept
    {
        amount.setTargetValue (juce::jlimit (0.0f, 1.0f, newAmount));
    }

    void DynamicsRestore::process (juce::dsp::AudioBlock<float>& block, const float* inputLoudness) noexcept
    {
        for (size_t i = 0; i < block.getNumSamples(); ++i)
        {
            inputDelay.pushSample (0, inputLoudness[i]);
            updateLevels (inputDelay.popSample (0), loudestSampleAt (block, i));

            const auto amountNow = amount.getNextValue();

            if (amountNow <= 0.0f)
                continue;

            const auto gain = restoringGain (amountNow);

            for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
                block.getChannelPointer (ch)[i] *= gain;
        }
    }

    void DynamicsRestore::updateLevels (float inputNow, float processedNow) noexcept
    {
        updateLevel (input, inputNow);
        updateLevel (processed, processedFollower.processSample (0, processedNow));
    }

    void DynamicsRestore::updateLevel (Levels& levels, float loudness) const noexcept
    {
        levels.nowDb = juce::Decibels::gainToDecibels (loudness, silenceDb);
        levels.averageDb += averageCoefficient * (levels.nowDb - levels.averageDb);
    }

    // Each signal's shape is how far it is above (+) or below (-) its own recent
    // average; the gain makes the processed shape match the input's.
    float DynamicsRestore::restoringGain (float amountNow) const noexcept
    {
        const auto inputShapeDb = input.nowDb - input.averageDb;
        const auto processedShapeDb = processed.nowDb - processed.averageDb;
        const auto correctionDb = juce::jlimit (maxCutDb, maxBoostDb, inputShapeDb - processedShapeDb);
        return juce::Decibels::decibelsToGain (amountNow * correctionDb);
    }
}
