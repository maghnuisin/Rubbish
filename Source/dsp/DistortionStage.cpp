#include "DistortionStage.h"

namespace rubbish
{
    void DistortionStage::prepare (const juce::dsp::ProcessSpec& spec)
    {
        jassert (spec.sampleRate > 0.0 && spec.maximumBlockSize > 0 && spec.numChannels > 0);

        maxChunkSize = spec.maximumBlockSize;

        oversampling = std::make_unique<juce::dsp::Oversampling<float>> (
            spec.numChannels,
            oversamplingStages,
            juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple,
            true,   // maximum quality filters
            true);  // whole-sample latency, so the host can compensate exactly
        oversampling->initProcessing (spec.maximumBlockSize);

        driveGain.prepare (spec);
        driveGain.setRampDurationSeconds (driveRampSeconds);

        const auto factor = oversampling->getOversamplingFactor();
        shaper.prepare (spec.sampleRate * static_cast<double> (factor), static_cast<int> (spec.numChannels));
        spread.prepare (spec.sampleRate * static_cast<double> (factor), spec.maximumBlockSize * factor);
        prepareDcBlocker (spec);
        reset();
    }

    void DistortionStage::prepareDcBlocker (const juce::dsp::ProcessSpec& spec)
    {
        *dcBlocker.state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass (spec.sampleRate, dcBlockerHz);
        dcBlocker.prepare (spec);
    }

    void DistortionStage::reset()
    {
        if (oversampling != nullptr)
            oversampling->reset();

        driveGain.reset();
        shaper.reset();
        spread.reset();
        dcBlocker.reset();
    }

    void DistortionStage::setDrive (const DriveState& newDrive) noexcept
    {
        driveGain.setGainDecibels (newDrive.totalDb);
        shaper.setDrive (newDrive);
        spread.setDrive (newDrive);
    }

    void DistortionStage::setCharacter (Character newCharacter) noexcept
    {
        shaper.setCharacter (newCharacter);
        spread.setCharacter (newCharacter);
    }

    void DistortionStage::setSpread (float newSpread) noexcept
    {
        spread.setAmount (newSpread);
    }

    int DistortionStage::getLatencyInSamples() const noexcept
    {
        jassert (oversampling != nullptr);
        return static_cast<int> (oversampling->getLatencyInSamples());
    }

    // Hosts occasionally send more samples than they promised in prepare(),
    // so work through the block in pieces the oversampler was sized for.
    void DistortionStage::process (juce::dsp::AudioBlock<float>& block) noexcept
    {
        jassert (oversampling != nullptr);

        const auto numSamples = block.getNumSamples();

        for (size_t start = 0; start < numSamples; start += maxChunkSize)
        {
            auto chunk = block.getSubBlock (start, juce::jmin (maxChunkSize, numSamples - start));
            processChunk (chunk);
        }
    }

    void DistortionStage::processChunk (juce::dsp::AudioBlock<float>& chunk) noexcept
    {
        driveGain.process (juce::dsp::ProcessContextReplacing<float> (chunk));

        auto oversampled = oversampling->processSamplesUp (chunk);
        spread.captureSides (oversampled);
        shaper.process (oversampled);
        spread.apply (oversampled);
        oversampling->processSamplesDown (chunk);

        dcBlocker.process (juce::dsp::ProcessContextReplacing<float> (chunk));
    }
}
