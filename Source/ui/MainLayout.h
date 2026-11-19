#pragma once

#include "DrivePanel.h"
#include "FilterOrderSwapButton.h"
#include "FiltersPanel.h"
#include "LevelMeter.h"
#include "OutputPanel.h"
#include "TwistPanel.h"

namespace rubbish
{
    /** Everything in the window, laid out at one fixed design size:
        the Twist strip across the top, then
        In meter | Drive | swap | Filters | Output (with Out meter).
        The editor scales this whole component up or down as the window resizes,
        so text, knobs and spacing always keep their proportions. */
    class MainLayout final : public juce::Component
    {
    public:
        MainLayout (juce::AudioProcessorValueTreeState& state,
                    LevelMeterSource& inputLevel,
                    LevelMeterSource& outputLevel,
                    TwistDisplaySource& twistPositions);

        void resized() override;

        static constexpr int designWidth = 1140;
        static constexpr int twistHeight = 420;
        static constexpr int lowerHeight = 250;
        static constexpr int designHeight = twistHeight + lowerHeight;

    private:
        void showFilterOrder (float orderIndex);
        void layOutLowerPanels (juce::Rectangle<int> area);

        static constexpr int panelGap = 10;

        TwistPanel twistPanel;
        LevelMeter inputMeter;
        DrivePanel drivePanel;
        FilterOrderSwapButton swapButton;
        FiltersPanel filtersPanel;
        OutputPanel outputPanel;

        bool screamOrder = false;

        // Scream puts the filters before the distortion, so the panels swap places to show it.
        juce::ParameterAttachment filterOrderWatcher;
    };
}
