#pragma once

#include "dsp/CleanLowSplitter.h"
#include "dsp/DistortionStage.h"
#include "dsp/DynamicsRestore.h"
#include "dsp/FilterFollow.h"
#include "dsp/FilterOrderSwitch.h"
#include "dsp/FilterSection.h"
#include "dsp/LevelMeterSource.h"
#include "dsp/LfoBank.h"
#include "dsp/ModulationMatrix.h"
#include "dsp/LoudnessTracker.h"
#include "dsp/OutputSection.h"
#include "dsp/TwistDisplaySource.h"
#include "dsp/TwistStage.h"
#include "TwistParameterReader.h"
#include "LfoParameterReader.h"

#include <juce_audio_processors/juce_audio_processors.h>

class RubbishAudioProcessor final : public juce::AudioProcessor
{
public:
    RubbishAudioProcessor();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void reset() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getValueTreeState() noexcept { return parameters; }

    /** Read-only levels for the window's meters. */
    rubbish::LevelMeterSource& getInputLevel() noexcept { return inputLevel; }
    rubbish::LevelMeterSource& getOutputLevel() noexcept { return outputLevel; }

    /** Read-only live bell positions for the Twist display. */
    rubbish::TwistDisplaySource& getTwistDisplaySource() noexcept { return twistDisplay; }

private:
    void updateDistortionSettings() noexcept;
    void updateFilterSettings() noexcept;
    void updateOutputSettings() noexcept;
    void updateSplitAndDynamicsSettings() noexcept;
    void updateTwistSettings() noexcept;
    void updateTwistTransport() noexcept;
    void updateLfoSettings() noexcept;
    rubbish::TwistStage::Movement getTwistMovement() const noexcept;
    void publishTwistPositions() noexcept;
    void updateAllSettings() noexcept;
    void processChunk (juce::dsp::AudioBlock<float>& block) noexcept;
    rubbish::FilterOrder getRequestedFilterOrder() const noexcept;
    void processDistortionAndFilters (juce::dsp::AudioBlock<float>& block, const float* highCutMultipliers) noexcept;
    void resetProcessing();

    juce::AudioProcessorValueTreeState parameters;

    std::atomic<float>& driveParam;
    std::atomic<float>& pushParam;
    std::atomic<float>& characterParam;
    std::atomic<float>& lowCutParam;
    std::atomic<float>& lowPeakParam;
    std::atomic<float>& highCutParam;
    std::atomic<float>& highPeakParam;
    std::atomic<float>& filterOrderParam;
    std::atomic<float>& autoGainParam;
    std::atomic<float>& mixParam;
    std::atomic<float>& outputParam;
    std::atomic<float>& cleanLowParam;
    std::atomic<float>& filterFollowParam;
    std::atomic<float>& dynamicsRestoreParam;
    std::atomic<float>& spreadParam;
    std::atomic<float>& deltaParam;
    rubbish::TwistParameterReader twistParams;
    rubbish::LfoParameterReader lfoParams;

    rubbish::LoudnessTracker inputLoudness;
    rubbish::CleanLowSplitter cleanLow;
    rubbish::FilterFollow filterFollow;
    rubbish::DistortionStage distortion;
    rubbish::FilterSection filters;
    rubbish::FilterOrderSwitch filterOrderSwitch;
    rubbish::TwistStage twist;
    rubbish::LfoBank lfos;
    rubbish::ModulationMatrix modulation;
    rubbish::DynamicsRestore dynamicsRestore;
    rubbish::OutputSection outputSection;
    rubbish::LevelMeterSource inputLevel;
    rubbish::LevelMeterSource outputLevel;
    rubbish::TwistDisplaySource twistDisplay;

    size_t maxChunkSize = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RubbishAudioProcessor)
};
