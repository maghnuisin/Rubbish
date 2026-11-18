#include "LabelledKnob.h"

namespace rubbish
{
    namespace
    {
        juce::RangedAudioParameter& getParameter (juce::AudioProcessorValueTreeState& state, const juce::String& id)
        {
            auto* parameter = state.getParameter (id);
            jassert (parameter != nullptr); // a parameter ID is missing from createParameterLayout()
            return *parameter;
        }

        double getDefaultValue (const juce::RangedAudioParameter& parameter)
        {
            return parameter.convertFrom0to1 (parameter.getDefaultValue());
        }
    }

    LabelledKnob::LabelledKnob (juce::AudioProcessorValueTreeState& state, const juce::String& parameterId,
                                const juce::String& displayName)
        : attachment (state, parameterId, knob)
    {
        const auto& parameter = getParameter (state, parameterId);

        nameLabel.setText (displayName.isNotEmpty() ? displayName : parameter.getName (64), juce::dontSendNotification);
        nameLabel.setJustificationType (juce::Justification::centred);
        addAndMakeVisible (nameLabel);

        knob.setSliderStyle (juce::Slider::RotaryVerticalDrag);
        knob.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, valueBoxHeight);
        knob.setDoubleClickReturnValue (true, getDefaultValue (parameter));
        addAndMakeVisible (knob);
    }

    void LabelledKnob::resized()
    {
        auto area = getLocalBounds();
        nameLabel.setBounds (area.removeFromTop (nameHeight));
        knob.setTextBoxStyle (juce::Slider::TextBoxBelow, false, juce::jmin (80, getWidth()), valueBoxHeight);
        knob.setBounds (area);
    }
}
