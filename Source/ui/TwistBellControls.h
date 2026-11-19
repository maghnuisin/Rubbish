#pragma once

#include "LabelledKnob.h"

#include <array>

namespace rubbish
{
    /** One Twist bell's controls in a single compact row:
        its coloured name, then Freq, Gain, Q, Speed, Move, Start knobs and the Shape menu.

        Speed is really two parameters (note lengths with Sync on, cycles per
        second with Sync off); only the one that applies is shown. */
    class TwistBellControls final : public juce::Component
    {
    public:
        TwistBellControls (juce::AudioProcessorValueTreeState& state, int bellIndex);

        void showSyncedSpeed (bool synced);
        void resized() override;

        /** Columns: name, Freq, Gain, Q, Speed, Move, Start, Shape. Shared with the
            column names above the rows so everything lines up. */
        static constexpr int numColumns = 8;
        static std::array<juce::Rectangle<int>, numColumns> getColumns (juce::Rectangle<int> row);

        static const juce::StringArray& getColumnNames();

    private:
        static constexpr int nameWidth = 64;
        static constexpr int shapeWidth = 130;

        juce::Label nameLabel;
        LabelledKnob freqKnob;
        LabelledKnob gainKnob;
        LabelledKnob qKnob;
        LabelledKnob syncSpeedKnob;
        LabelledKnob freeSpeedKnob;
        LabelledKnob moveKnob;
        LabelledKnob startKnob;

        juce::ComboBox shapeMenu;
        juce::AudioProcessorValueTreeState::ComboBoxAttachment shapeAttachment;
    };
}
