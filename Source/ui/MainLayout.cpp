#include "MainLayout.h"
#include "PanelStyle.h"
#include "Parameters.h"

namespace rubbish
{
    MainLayout::MainLayout (juce::AudioProcessorValueTreeState& state,
                            LevelMeterSource& inputLevel,
                            LevelMeterSource& outputLevel,
                            TwistDisplaySource& twistPositions)
        : twistPanel (state, twistPositions),
          inputMeter (inputLevel, "In"),
          drivePanel (state),
          swapButton (*state.getParameter (ParamIDs::filterOrder)),
          filtersPanel (state),
          outputPanel (state, outputLevel),
          filterOrderWatcher (*state.getParameter (ParamIDs::filterOrder),
                              [this] (float orderIndex) { showFilterOrder (orderIndex); })
    {
        for (auto* child : std::initializer_list<juce::Component*> { &twistPanel, &inputMeter, &drivePanel, &swapButton,
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
        twistPanel.setBounds (area.removeFromTop (twistHeight - panelGap));
        area.removeFromTop (panelGap);
        layOutLowerPanels (area);
    }

    // In meter | first panel | swap | second panel | Output. Scream puts Filters first.
    void MainLayout::layOutLowerPanels (juce::Rectangle<int> area)
    {
        inputMeter.setBounds (area.removeFromLeft (LevelMeter::preferredWidth)
                                  .withTrimmedTop (panelStyle::titleHeight)
                                  .withTrimmedBottom (panelStyle::padding));
        area.removeFromLeft (panelGap);

        outputPanel.setBounds (area.removeFromRight (OutputPanel::preferredWidth));
        area.removeFromRight (panelGap);

        auto& firstPanel = screamOrder ? static_cast<juce::Component&> (filtersPanel) : drivePanel;
        auto& secondPanel = screamOrder ? static_cast<juce::Component&> (drivePanel) : filtersPanel;
        const auto firstWidth = screamOrder ? FiltersPanel::preferredWidth : DrivePanel::preferredWidth;

        firstPanel.setBounds (area.removeFromLeft (firstWidth));
        area.removeFromLeft (panelGap);
        swapButton.setBounds (area.removeFromLeft (FilterOrderSwapButton::preferredWidth)
                                  .withSizeKeepingCentre (FilterOrderSwapButton::preferredWidth,
                                                          FilterOrderSwapButton::preferredHeight));
        area.removeFromLeft (panelGap);
        secondPanel.setBounds (area);
    }
}
