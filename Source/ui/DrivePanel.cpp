#include "DrivePanel.h"
#include "PanelStyle.h"
#include "Parameters.h"

namespace rubbish
{
    namespace
    {
        // The menu needs its items before the attachment connects it to the parameter.
        // ComboBox item IDs must not be zero, so they start at 1.
        juce::ComboBox& withCharacterItems (juce::ComboBox& menu, juce::AudioProcessorValueTreeState& state)
        {
            auto* character = dynamic_cast<juce::AudioParameterChoice*> (state.getParameter (ParamIDs::character));
            jassert (character != nullptr); // Character must be a choice parameter
            menu.addItemList (character != nullptr ? character->choices : juce::StringArray(), 1);
            return menu;
        }
    }

    DrivePanel::DrivePanel (juce::AudioProcessorValueTreeState& state)
        : driveKnob (state, ParamIDs::drive),
          cleanLowKnob (state, ParamIDs::cleanLow),
          characterAttachment (state, ParamIDs::character, withCharacterItems (characterMenu, state)),
          pushAttachment (state, ParamIDs::push, pushButton)
    {
        pushButton.setClickingTogglesState (true);

        characterLabel.setText ("Character", juce::dontSendNotification);
        characterLabel.setJustificationType (juce::Justification::centred);

        for (auto* child : std::initializer_list<juce::Component*> { &driveKnob, &characterLabel, &characterMenu,
                                                                     &pushButton, &cleanLowKnob })
            addAndMakeVisible (child);
    }

    void DrivePanel::paint (juce::Graphics& g)
    {
        panelStyle::drawFrame (g, getLocalBounds(), "Drive");
    }

    void DrivePanel::resized()
    {
        auto area = panelStyle::getContentArea (getLocalBounds());
        layOutSideColumn (area.removeFromRight (sideColumnWidth));
        area.removeFromRight (6);
        pushButton.setBounds (area.removeFromBottom (28).reduced (8, 0));
        area.removeFromBottom (6);
        driveKnob.setBounds (area);
    }

    void DrivePanel::layOutSideColumn (juce::Rectangle<int> area)
    {
        characterLabel.setBounds (area.removeFromTop (20));
        characterMenu.setBounds (area.removeFromTop (26));
        area.removeFromTop (10);

        // Kept small so Drive stays the biggest knob in the panel.
        cleanLowKnob.setBounds (area.removeFromBottom (juce::jmin (area.getHeight(), 118)));
    }
}
