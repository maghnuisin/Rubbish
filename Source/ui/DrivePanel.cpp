#include "DrivePanel.h"
#include "PanelStyle.h"
#include "Parameters.h"

namespace rubbish
{
    DrivePanel::DrivePanel (juce::AudioProcessorValueTreeState& state)
        : driveKnob (state, ParamIDs::drive),
          characterButtons (*state.getParameter (ParamIDs::character), 2),
          cleanLowKnob (state, ParamIDs::cleanLow),
          pushAttachment (state, ParamIDs::push, pushButton)
    {
        pushButton.setClickingTogglesState (true);

        characterLabel.setText ("Character", juce::dontSendNotification);
        characterLabel.setJustificationType (juce::Justification::centred);

        addAndMakeVisible (driveKnob);
        addAndMakeVisible (pushButton);
        addAndMakeVisible (characterLabel);
        addAndMakeVisible (characterButtons);
        addAndMakeVisible (cleanLowKnob);
    }

    void DrivePanel::paint (juce::Graphics& g)
    {
        panelStyle::drawFrame (g, getLocalBounds(), "Drive");
    }

    void DrivePanel::resized()
    {
        auto area = panelStyle::getContentArea (getLocalBounds());

        const auto cleanLowHeight = juce::jmin (120, area.getHeight() / 4);
        cleanLowKnob.setBounds (area.removeFromBottom (cleanLowHeight).withSizeKeepingCentre (110, cleanLowHeight));
        area.removeFromBottom (8);

        auto characterArea = area.removeFromBottom (juce::jmin (100, area.getHeight() / 4));
        characterLabel.setBounds (area.removeFromBottom (22));
        characterButtons.setBounds (characterArea);

        area.removeFromBottom (8);
        pushButton.setBounds (area.removeFromBottom (30).withSizeKeepingCentre (100, 30));

        area.removeFromBottom (8);
        driveKnob.setBounds (area);
    }
}
