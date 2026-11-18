#include "MainLayout.h"
#include "PanelStyle.h"
#include "Parameters.h"

namespace rubbish
{
    MainLayout::MainLayout (juce::AudioProcessorValueTreeState& state,
                            LevelMeterSource& inputLevel,
                            LevelMeterSource& outputLevel)
        : inputMeter (inputLevel, "In"),
          drivePanel (state),
          swapButton (*state.getParameter (ParamIDs::filterOrder)),
          filtersPanel (state),
          outputPanel (state, outputLevel),
          filterOrderWatcher (*state.getParameter (ParamIDs::filterOrder),
                              [this] (float orderIndex) { showFilterOrder (orderIndex); })
    {
        for (auto* child : std::initializer_list<juce::Component*> { &inputMeter, &drivePanel, &swapButton,
                                                                     &filtersPanel, &outputPanel })
            addAndMakeVisible (child);

        filterOrderWatcher.sendInitialUpdate();
    }

    void MainLayout::showFilterOrder (float orderIndex)
    {
        screamOrder = juce::roundToInt (orderIndex) == 1;
        resized();
    }

    void MainLayout::resized()
    {
        auto area = getLocalBounds().reduced (panelGap);
        inputMeter.setBounds (area.removeFromLeft (LevelMeter::preferredWidth)
                                  .withTrimmedTop (panelStyle::titleHeight)
                                  .withTrimmedBottom (panelStyle::padding));
        area.removeFromLeft (panelGap);

        outputPanel.setBounds (area.removeFromRight (area.getWidth() / 3));
        area.removeFromRight (panelGap);

        auto leftArea = area.removeFromLeft ((area.getWidth() - FilterOrderSwapButton::preferredWidth - 2 * panelGap) / 2);
        area.removeFromLeft (panelGap);
        swapButton.setBounds (area.removeFromLeft (FilterOrderSwapButton::preferredWidth)
                                  .withSizeKeepingCentre (FilterOrderSwapButton::preferredWidth,
                                                          FilterOrderSwapButton::preferredHeight));
        area.removeFromLeft (panelGap);

        auto& leftPanel = screamOrder ? static_cast<juce::Component&> (filtersPanel) : drivePanel;
        auto& rightPanel = screamOrder ? static_cast<juce::Component&> (drivePanel) : filtersPanel;
        leftPanel.setBounds (leftArea);
        rightPanel.setBounds (area);
    }
}
