#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters.h"
#include "StateMigration.h"

namespace
{
    std::atomic<float>& getRawParameter (juce::AudioProcessorValueTreeState& state, const char* id)
    {
        auto* value = state.getRawParameterValue (id);
        jassert (value != nullptr); // a parameter ID is missing from createParameterLayout()
        return *value;
    }
}

RubbishAudioProcessor::RubbishAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "RubbishState", rubbish::createParameterLayout()),
      driveParam (getRawParameter (parameters, rubbish::ParamIDs::drive)),
      pushParam (getRawParameter (parameters, rubbish::ParamIDs::push)),
      characterParam (getRawParameter (parameters, rubbish::ParamIDs::character)),
      lowCutParam (getRawParameter (parameters, rubbish::ParamIDs::lowCut)),
      lowPeakParam (getRawParameter (parameters, rubbish::ParamIDs::lowPeak)),
      highCutParam (getRawParameter (parameters, rubbish::ParamIDs::highCut)),
      highPeakParam (getRawParameter (parameters, rubbish::ParamIDs::highPeak)),
      filterOrderParam (getRawParameter (parameters, rubbish::ParamIDs::filterOrder)),
      autoGainParam (getRawParameter (parameters, rubbish::ParamIDs::autoGain)),
      mixParam (getRawParameter (parameters, rubbish::ParamIDs::mix)),
      outputParam (getRawParameter (parameters, rubbish::ParamIDs::output)),
      cleanLowParam (getRawParameter (parameters, rubbish::ParamIDs::cleanLow)),
      filterFollowParam (getRawParameter (parameters, rubbish::ParamIDs::filterFollow)),
      dynamicsRestoreParam (getRawParameter (parameters, rubbish::ParamIDs::dynamicsRestore)),
      spreadParam (getRawParameter (parameters, rubbish::ParamIDs::spread)),
      deltaParam (getRawParameter (parameters, rubbish::ParamIDs::delta)),
      twistParams (parameters),
      lfoParams (parameters)
{
    rubbish::markCurrentCharacterChoices (parameters.state);
}

void RubbishAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    const juce::dsp::ProcessSpec spec { sampleRate,
                                        static_cast<juce::uint32> (samplesPerBlock),
                                        static_cast<juce::uint32> (getTotalNumOutputChannels()) };
    maxChunkSize = spec.maximumBlockSize;

    distortion.prepare (spec);
    const auto latency = distortion.getLatencyInSamples();

    inputLoudness.prepare (spec);
    cleanLow.prepare (spec, latency);
    filterFollow.prepare (sampleRate, spec.maximumBlockSize);
    filters.prepare (spec);
    filterOrderSwitch.prepare (sampleRate);
    twist.prepare (spec);
    lfos.prepare (sampleRate, samplesPerBlock);
    modulation.prepare (sampleRate, samplesPerBlock);
    dynamicsRestore.prepare (spec, latency);
    outputSection.prepare (spec, latency);

    updateAllSettings();
    resetProcessing(); // start at the current settings rather than ramping from defaults
    setLatencySamples (latency);
}

void RubbishAudioProcessor::releaseResources() {}

void RubbishAudioProcessor::reset()
{
    resetProcessing();
}

void RubbishAudioProcessor::resetProcessing()
{
    inputLoudness.reset();
    cleanLow.reset();
    filterFollow.reset();
    dynamicsRestore.reset();
    distortion.reset();
    filters.reset();
    filterOrderSwitch.reset (getRequestedFilterOrder());
    twist.reset();
    lfos.reset();
    modulation.reset();
    outputSection.reset();
}

bool RubbishAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto output = layouts.getMainOutputChannelSet();

    if (output != juce::AudioChannelSet::mono() && output != juce::AudioChannelSet::stereo())
        return false;

    return output == layouts.getMainInputChannelSet();
}

void RubbishAudioProcessor::updateDistortionSettings() noexcept
{
    const auto pushOn = pushParam.load() >= 0.5f;
    const auto driveDb = driveParam.load();
    distortion.setDrive ({ driveDb,
                           driveDb + (pushOn ? rubbish::pushBoostDb : 0.0f),
                           pushOn ? 1.0f : 0.0f });

    const auto characterIndex = juce::jlimit (0, rubbish::numCharacters - 1,
                                              juce::roundToInt (characterParam.load()));
    distortion.setCharacter (static_cast<rubbish::Character> (characterIndex));

    constexpr float percentToFraction = 0.01f;
    distortion.setSpread (spreadParam.load() * percentToFraction);
}

void RubbishAudioProcessor::updateSplitAndDynamicsSettings() noexcept
{
    constexpr float percentToFraction = 0.01f;
    cleanLow.setCutoffHz (cleanLowParam.load());
    dynamicsRestore.setAmount (dynamicsRestoreParam.load() * percentToFraction);
}

void RubbishAudioProcessor::updateAllSettings() noexcept
{
    updateDistortionSettings();
    updateFilterSettings();
    updateSplitAndDynamicsSettings();
    updateTwistSettings();
    updateLfoSettings();
    updateOutputSettings();
}

void RubbishAudioProcessor::updateTwistSettings() noexcept
{
    twist.setEnabled (twistParams.isEnabled());

    for (int bell = 0; bell < rubbish::TwistStage::numBells; ++bell)
        twist.setBell (bell, twistParams.readBell (bell));
}

void RubbishAudioProcessor::updateLfoSettings() noexcept
{
    for (int lfo = 0; lfo < rubbish::LfoBank::numLfos; ++lfo)
    {
        lfos.setLfo (lfo, lfoParams.readLfo (lfo));

        for (int target = 0; target < rubbish::ModulationMatrix::numTargets; ++target)
            modulation.setAmount (lfo, target, lfoParams.readAmount (lfo, target));
    }
}

rubbish::TwistStage::Movement RubbishAudioProcessor::getTwistMovement() const noexcept
{
    rubbish::TwistStage::Movement movement;

    for (int bell = 0; bell < rubbish::TwistStage::numBells; ++bell)
        movement[static_cast<size_t> (bell)] = { modulation.getModulation (rubbish::bellFreqTarget (bell)),
                                                 modulation.getModulation (rubbish::bellGainTarget (bell)),
                                                 modulation.getModulation (rubbish::bellQTarget (bell)) };

    return movement;
}

void RubbishAudioProcessor::publishTwistPositions() noexcept
{
    for (int bell = 0; bell < rubbish::TwistStage::numBells; ++bell)
        twistDisplay.publishFrequency (bell, twist.getCurrentFrequencyHz (bell));
}

// The LFOs' Sync follows the DAW's tempo and song position.
void RubbishAudioProcessor::updateTwistTransport() noexcept
{
    rubbish::TwistTransport transport;

    if (auto* playHead = getPlayHead())
    {
        if (const auto position = playHead->getPosition())
        {
            transport.bpm = position->getBpm().orFallback (120.0);
            transport.isPlaying = position->getIsPlaying();

            if (const auto ppq = position->getPpqPosition())
                transport.songPositionBeats = *ppq;

            if (const auto timeSignature = position->getTimeSignature())
                transport.quarterNotesPerBar = 4.0 * timeSignature->numerator / juce::jmax (1, timeSignature->denominator);
        }
    }

    lfos.setTransport (transport);
}

void RubbishAudioProcessor::updateFilterSettings() noexcept
{
    constexpr float percentToFraction = 0.01f;
    filters.setLowCut (lowCutParam.load(), lowPeakParam.load() * percentToFraction);
    filters.setHighCut (highCutParam.load(), highPeakParam.load() * percentToFraction);
    filterFollow.setAmount (filterFollowParam.load() * percentToFraction);
}

void RubbishAudioProcessor::updateOutputSettings() noexcept
{
    constexpr float percentToFraction = 0.01f;
    outputSection.setAutoGainEnabled (autoGainParam.load() >= 0.5f);
    outputSection.setMix (mixParam.load() * percentToFraction);
    outputSection.setOutputDecibels (outputParam.load());
    outputSection.setDeltaEnabled (deltaParam.load() >= 0.5f);
}

rubbish::FilterOrder RubbishAudioProcessor::getRequestedFilterOrder() const noexcept
{
    return filterOrderParam.load() >= 0.5f ? rubbish::FilterOrder::scream
                                           : rubbish::FilterOrder::cleanFirst;
}

void RubbishAudioProcessor::processDistortionAndFilters (juce::dsp::AudioBlock<float>& block,
                                                         const float* highCutMultipliers) noexcept
{
    filterOrderSwitch.setRequestedOrder (getRequestedFilterOrder());

    if (filterOrderSwitch.getActiveOrder() == rubbish::FilterOrder::cleanFirst)
    {
        distortion.process (block);
        filters.process (block, highCutMultipliers);
    }
    else
    {
        filters.process (block, highCutMultipliers);
        distortion.process (block);
    }

    filterOrderSwitch.applyFade (block);
}

void RubbishAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    for (auto ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());

    updateAllSettings();
    updateTwistTransport();

    // Hosts occasionally send more samples than promised in prepareToPlay(),
    // so work through the buffer in pieces everything was sized for.
    jassert (maxChunkSize > 0); // prepareToPlay() must run first

    if (maxChunkSize == 0)
        return;

    juce::dsp::AudioBlock<float> block (buffer);
    const auto numSamples = block.getNumSamples();

    for (size_t start = 0; start < numSamples; start += maxChunkSize)
    {
        auto chunk = block.getSubBlock (start, juce::jmin (maxChunkSize, numSamples - start));
        processChunk (chunk);
    }
}

// Signal flow follows prd.md: Clean Low split -> distortion and filters -> Twist ->
// recombine low band -> Dynamics Restore -> Auto Gain -> Mix -> Output -> limiter.
void RubbishAudioProcessor::processChunk (juce::dsp::AudioBlock<float>& block) noexcept
{
    inputLevel.measure (block);
    inputLoudness.process (block);
    outputSection.captureDry (block);

    const auto* highCutMultipliers = filterFollow.computeCutoffMultipliers (inputLoudness.getEnvelope(),
                                                                            block.getNumSamples());
    const auto numSamples = static_cast<int> (block.getNumSamples());
    lfos.render (numSamples);
    modulation.process (lfos, numSamples);

    cleanLow.split (block);
    processDistortionAndFilters (block, highCutMultipliers);
    twist.process (block, getTwistMovement());
    publishTwistPositions();
    cleanLow.recombine (block);

    dynamicsRestore.process (block, inputLoudness.getEnvelope());
    outputSection.process (block);
    outputLevel.measure (block);
}

juce::AudioProcessorEditor* RubbishAudioProcessor::createEditor()
{
    return new RubbishAudioProcessorEditor (*this);
}

void RubbishAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = parameters.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void RubbishAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    const auto xml = getXmlFromBinary (data, sizeInBytes);

    if (xml == nullptr || ! xml->hasTagName (parameters.state.getType()))
        return;

    auto state = juce::ValueTree::fromXml (*xml);
    rubbish::upgradeVersion2State (state);
    rubbish::upgradeCharacterChoices (state);
    parameters.replaceState (state);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RubbishAudioProcessor();
}
