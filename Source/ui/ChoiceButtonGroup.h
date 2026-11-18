#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace rubbish
{
    /** One labelled button per choice of a choice parameter, laid out in a grid.
        Exactly one is lit, and it stays in sync with the parameter. */
    class ChoiceButtonGroup final : public juce::Component
    {
    public:
        ChoiceButtonGroup (juce::RangedAudioParameter& choiceParameter, int numColumns);

        void resized() override;

    private:
        void createButtons (const juce::StringArray& names);
        void selectButton (int index);

        static constexpr int buttonGap = 4;

        const int columns;
        juce::OwnedArray<juce::TextButton> buttons;
        juce::ParameterAttachment attachment;
    };
}
