#pragma once

#include "LabelledKnob.h"

#include <array>

namespace rubbish
{
    /** One Twist bell's controls in a single compact row:
        its coloured name, then Freq, Gain, Q, Speed, Move, Start knobs, the Shape menu
        and a Sync toggle.

        Until the LFO section is on screen (version 3 stage 2), Speed, Move, Start, Shape
        and Sync control the bell's matching LFO; Move is that LFO's ring on the bell's Freq.

        Speed is really two parameters (note lengths with Sync on, cycles per
        second with Sync off); only the one that applies is shown. */
    class TwistBellControls final : public juce::Component
    {
    public:
        TwistBellControls (juce::AudioProcessorValueTreeState& state, int bellIndex);

        void resized() override;

        /** Columns: name, Freq, Gain, Q, Speed, Move, Start, Shape, Sync. Shared with the
            column names above the rows so everything lines up. */
        static constexpr int numColumns = 9;
        static std::array<juce::Rectangle<int>, numColumns> getColumns (juce::Rectangle<int> row);

        static const juce::StringArray& getColumnNames();

    private:
        static constexpr int nameWidth = 64;
        static constexpr int shapeWidth = 130;
        static constexpr int syncWidth = 64;

        void showSyncedSpeed (bool synced);

        juce::Label nameLabel;
        LabelledKnob freqKnob;
        LabelledKnob gainKnob;
        LabelledKnob qKnob;
        LabelledKnob syncSpeedKnob;
        LabelledKnob freeSpeedKnob;
        LabelledKnob moveKnob;
        LabelledKnob startKnob;

        juce::ComboBox shapeMenu;
        juce::TextButton syncButton { "Sync" };
        juce::AudioProcessorValueTreeState::ComboBoxAttachment shapeAttachment;
        juce::AudioProcessorValueTreeState::ButtonAttachment syncAttachment;
        juce::ParameterAttachment syncWatcher;
    };
}
