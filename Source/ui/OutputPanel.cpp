#include "OutputPanel.h"
#include "PanelStyle.h"
#include "Parameters.h"

namespace rubbish
{
    OutputPanel::OutputPanel (juce::AudioProcessorValueTreeState& state, LevelMeterSource& outputLevel)
        : dynamicsRestoreKnob (state, ParamIDs::dynamicsRestore, "Dyn. Restore"),
          spreadKnob (state, ParamIDs::spread),
          mixKnob (state, ParamIDs::mix),
          outputKnob (state, ParamIDs::output),
          meter (outputLevel, "Out"),
          autoGainAttachment (state, ParamIDs::autoGain, autoGainButton),
          deltaAttachment (state, ParamIDs::delta, deltaButton)
    {
        autoGainButton.setClickingTogglesState (true);
        deltaButton.setClickingTogglesState (true);

        for (auto* child : std::initializer_list<juce::Component*> { &dynamicsRestoreKnob, &spreadKnob, &mixKnob,
                                                                     &outputKnob, &autoGainButton, &deltaButton, &meter })
            addAndMakeVisible (child);
    }

    void OutputPanel::paint (juce::Graphics& g)
    {
        panelStyle::drawFrame (g, getLocalBounds(), "Output");
    }

    void OutputPanel::resized()
    {
        auto area = panelStyle::getContentArea (getLocalBounds());

        meter.setBounds (area.removeFromRight (LevelMeter::preferredWidth));
        area.removeFromRight (panelStyle::padding);

        layOutToggles (area.removeFromBottom (30));
        area.removeFromBottom (8);
        layOutKnobs (area);
    }

    void OutputPanel::layOutKnobs (juce::Rectangle<int> area)
    {
        const auto knobWidth = area.getWidth() / 4;

        for (auto* knob : { &dynamicsRestoreKnob, &spreadKnob, &mixKnob })
            knob->setBounds (area.removeFromLeft (knobWidth).reduced (3, 0));

        outputKnob.setBounds (area.reduced (3, 0));
    }

    void OutputPanel::layOutToggles (juce::Rectangle<int> area)
    {
        autoGainButton.setBounds (area.removeFromLeft (area.getWidth() / 2).reduced (4, 0));
        deltaButton.setBounds (area.reduced (4, 0));
    }
}
