#pragma once

#include "LabelledKnob.h"

namespace rubbish
{
    /** Drive (the biggest knob in the lower panels) with Push beneath it, and
        beside them the Character menu and the small Clean Low knob. */
    class DrivePanel final : public juce::Component
    {
    public:
        explicit DrivePanel (juce::AudioProcessorValueTreeState& state);

        void paint (juce::Graphics& g) override;
        void resized() override;

        static constexpr int preferredWidth = 230;

    private:
        static constexpr int sideColumnWidth = 94;

        void layOutSideColumn (juce::Rectangle<int> area);

        LabelledKnob driveKnob;
        juce::Label characterLabel;
        juce::ComboBox characterMenu;
        juce::TextButton pushButton { "Push" };
        LabelledKnob cleanLowKnob;

        juce::AudioProcessorValueTreeState::ComboBoxAttachment characterAttachment;
        juce::AudioProcessorValueTreeState::ButtonAttachment pushAttachment;
    };
}
