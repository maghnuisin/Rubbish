#pragma once

#include "TwistBellControls.h"
#include "TwistDisplay.h"

#include <array>
#include <memory>

namespace rubbish
{
    /** The Twist strip: the moving display across the top, and underneath it the
        Twist and Sync toggles, Bell 1 / 2 / 3 tabs, and the selected bell's controls. */
    class TwistPanel final : public juce::Component
    {
    public:
        TwistPanel (juce::AudioProcessorValueTreeState& state, TwistDisplaySource& bellPositions);

        void paint (juce::Graphics& g) override;
        void resized() override;

    private:
        static constexpr int numBells = 3;
        static constexpr int toggleWidth = 90;
        static constexpr int tabWidth = 70;
        static constexpr int controlsHeight = 110;

        void configureTabs();
        void selectBell (int bellIndex);
        void showSyncMode (float syncValue);

        TwistDisplay display;
        juce::TextButton twistButton { "Twist" };
        juce::TextButton syncButton { "Sync" };
        std::array<juce::TextButton, numBells> bellTabs;
        std::array<std::unique_ptr<TwistBellControls>, numBells> bellControls;

        juce::AudioProcessorValueTreeState::ButtonAttachment twistAttachment;
        juce::AudioProcessorValueTreeState::ButtonAttachment syncAttachment;
        juce::ParameterAttachment syncWatcher;
    };
}
