#pragma once

#include "LabelledKnob.h"
#include "LevelMeter.h"

namespace rubbish
{
    /** Dynamics Restore, Spread, Mix and Output knobs, the Auto Gain and Delta
        toggles, and the output level meter. */
    class OutputPanel final : public juce::Component
    {
    public:
        OutputPanel (juce::AudioProcessorValueTreeState& state, LevelMeterSource& outputLevel);

        void paint (juce::Graphics& g) override;
        void resized() override;

    private:
        void layOutKnobs (juce::Rectangle<int> area);
        void layOutToggles (juce::Rectangle<int> area);

        LabelledKnob dynamicsRestoreKnob;
        LabelledKnob spreadKnob;
        LabelledKnob mixKnob;
        LabelledKnob outputKnob;
        juce::TextButton autoGainButton { "Auto Gain" };
        juce::TextButton deltaButton { "Delta" };
        LevelMeter meter;

        juce::AudioProcessorValueTreeState::ButtonAttachment autoGainAttachment;
        juce::AudioProcessorValueTreeState::ButtonAttachment deltaAttachment;
    };
}
