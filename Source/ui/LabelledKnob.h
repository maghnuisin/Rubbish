#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "FineDragSlider.h"

#include <juce_gui_basics/juce_gui_basics.h>

namespace rubbish
{
    /** A rotary knob with its name above and its value (with unit) below,
        attached to one parameter.

        Drag up/down to change it; hold Shift, Cmd or Ctrl for fine adjustment;
        double-click to reset; scroll to nudge; click the value to type one. */
    class LabelledKnob final : public juce::Component
    {
    public:
        LabelledKnob (juce::AudioProcessorValueTreeState& state, const char* parameterId);

        void resized() override;

    private:
        static constexpr int nameHeight = 20;
        static constexpr int valueBoxHeight = 20;

        juce::Label nameLabel;
        FineDragSlider knob;
        juce::AudioProcessorValueTreeState::SliderAttachment attachment;
    };
}
