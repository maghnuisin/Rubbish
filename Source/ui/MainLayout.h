#pragma once

#include "DrivePanel.h"
#include "FilterOrderSwapButton.h"
#include "FiltersPanel.h"
#include "LevelMeter.h"
#include "OutputPanel.h"

namespace rubbish
{
    /** Everything in the window, laid out at one fixed design size:
        In meter | Drive | swap | Filters | Output (with Out meter).
        The editor scales this whole component up or down as the window resizes,
        so text, knobs and spacing always keep their proportions. */
    class MainLayout final : public juce::Component
    {
    public:
        MainLayout (juce::AudioProcessorValueTreeState& state,
                    LevelMeterSource& inputLevel,
                    LevelMeterSource& outputLevel);

        void resized() override;

        static constexpr int designWidth = 1020;
        static constexpr int designHeight = 560;

    private:
        void showFilterOrder (float orderIndex);

        static constexpr int panelGap = 10;

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
