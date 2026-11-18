#pragma once

#include "ChoiceButtonGroup.h"
#include "LabelledKnob.h"

namespace rubbish
{
    /** Drive (the biggest knob), Push beneath it, the four Character buttons,
        and the small Clean Low knob. */
    class DrivePanel final : public juce::Component
    {
    public:
        explicit DrivePanel (juce::AudioProcessorValueTreeState& state);

        void paint (juce::Graphics& g) override;
        void resized() override;

    private:
        LabelledKnob driveKnob;
        juce::TextButton pushButton { "Push" };
        juce::Label characterLabel;
        ChoiceButtonGroup characterButtons;
        LabelledKnob cleanLowKnob;

        juce::AudioProcessorValueTreeState::ButtonAttachment pushAttachment;
    };
}
