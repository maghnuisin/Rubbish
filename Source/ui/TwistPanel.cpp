#include "TwistPanel.h"
#include "PanelStyle.h"
#include "Parameters.h"
#include "TwistColours.h"

namespace rubbish
{
    namespace
    {
        constexpr int bellTabRadioGroup = 4201;
    }

    TwistPanel::TwistPanel (juce::AudioProcessorValueTreeState& state, TwistDisplaySource& bellPositions)
        : display (state, bellPositions),
          twistAttachment (state, ParamIDs::twist, twistButton),
          syncAttachment (state, ParamIDs::twistSync, syncButton),
          syncWatcher (*state.getParameter (ParamIDs::twistSync), [this] (float value) { showSyncMode (value); })
    {
        twistButton.setClickingTogglesState (true);
        syncButton.setClickingTogglesState (true);
        display.onBellPicked = [this] (int bell) { selectBell (bell); };
        addAndMakeVisible (display);
        addAndMakeVisible (twistButton);
        addAndMakeVisible (syncButton);

        for (int bell = 0; bell < numBells; ++bell)
        {
            bellControls[static_cast<size_t> (bell)] = std::make_unique<TwistBellControls> (state, bell);
            addChildComponent (*bellControls[static_cast<size_t> (bell)]);
        }

        configureTabs();
        selectBell (0);
        syncWatcher.sendInitialUpdate();
    }

    void TwistPanel::configureTabs()
    {
        for (int bell = 0; bell < numBells; ++bell)
        {
            auto& tab = bellTabs[static_cast<size_t> (bell)];
            tab.setButtonText ("Bell " + juce::String (bell + 1));
            tab.setClickingTogglesState (true);
            tab.setRadioGroupId (bellTabRadioGroup);
            tab.setColour (juce::TextButton::buttonOnColourId, getTwistBellColour (bell));
            tab.onClick = [this, bell] { selectBell (bell); };
            addAndMakeVisible (tab);
        }
    }

    // Which bell is showing is just a view choice, not a sound setting, so it isn't a parameter.
    void TwistPanel::selectBell (int bellIndex)
    {
        for (int bell = 0; bell < numBells; ++bell)
        {
            const auto isSelected = bell == bellIndex;
            bellTabs[static_cast<size_t> (bell)].setToggleState (isSelected, juce::dontSendNotification);
            bellControls[static_cast<size_t> (bell)]->setVisible (isSelected);
        }

        display.setSelectedBell (bellIndex);
    }

    void TwistPanel::showSyncMode (float syncValue)
    {
        for (auto& controls : bellControls)
            controls->showSyncedSpeed (syncValue >= 0.5f);
    }

    void TwistPanel::paint (juce::Graphics& g)
    {
        panelStyle::drawFrame (g, getLocalBounds(), "Twist");
    }

    void TwistPanel::resized()
    {
        auto area = panelStyle::getContentArea (getLocalBounds());
        auto controlsArea = area.removeFromBottom (controlsHeight);
        area.removeFromBottom (6);
        display.setBounds (area);
        area = controlsArea;

        auto toggles = area.removeFromLeft (toggleWidth).withSizeKeepingCentre (toggleWidth, 70);
        twistButton.setBounds (toggles.removeFromTop (30));
        syncButton.setBounds (toggles.removeFromBottom (30));
        area.removeFromLeft (panelStyle::padding);

        auto tabs = area.removeFromLeft (tabWidth).withSizeKeepingCentre (tabWidth, 100);
        for (auto& tab : bellTabs)
            tab.setBounds (tabs.removeFromTop (30).reduced (0, 1));
        area.removeFromLeft (panelStyle::padding);

        for (auto& controls : bellControls)
            controls->setBounds (area);
    }
}
