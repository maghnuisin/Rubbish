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
        /** displayName replaces the parameter's own name above the knob, if given. */
        LabelledKnob (juce::AudioProcessorValueTreeState& state, const juce::String& parameterId,
                      const juce::String& displayName = {});

        /** Small version for a row of controls under shared column names:
            no name above, value box to the right of the knob. */
        void useRowStyle();

        void resized() override;

    private:
        static constexpr int nameHeight = 20;
        static constexpr int valueBoxHeight = 20;
        static constexpr int rowValueBoxWidth = 62;

        bool rowStyle = false;

        juce::Label nameLabel;
        FineDragSlider knob;
        juce::AudioProcessorValueTreeState::SliderAttachment attachment;
    };
}
