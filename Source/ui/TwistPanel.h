#pragma once

#include "TwistBellControls.h"
#include "TwistDisplay.h"

#include <array>
#include <memory>

namespace rubbish
{
    /** The Twist strip: the moving display across the top, and underneath it the
        Twist and Sync toggles beside three rows of controls, one row per bell. */
    class TwistPanel final : public juce::Component
    {
    public:
        TwistPanel (juce::AudioProcessorValueTreeState& state, TwistDisplaySource& bellPositions);

        void paint (juce::Graphics& g) override;
        void resized() override;

    private:
        static constexpr int numBells = 3;
        static constexpr int toggleWidth = 90;
        static constexpr int headerHeight = 18;
        static constexpr int rowHeight = 50;
        static constexpr int controlsHeight = headerHeight + numBells * rowHeight;

        void createColumnNames();
        void showSyncMode (float syncValue);
        void layOutControls (juce::Rectangle<int> area);

        TwistDisplay display;
        juce::TextButton twistButton { "Twist" };
        juce::TextButton syncButton { "Sync" };
        juce::OwnedArray<juce::Label> columnNames;
        std::array<std::unique_ptr<TwistBellControls>, numBells> bellRows;

        juce::AudioProcessorValueTreeState::ButtonAttachment twistAttachment;
        juce::AudioProcessorValueTreeState::ButtonAttachment syncAttachment;
        juce::ParameterAttachment syncWatcher;
    };
}
