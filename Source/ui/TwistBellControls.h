#pragma once

#include "LabelledKnob.h"

namespace rubbish
{
    /** One Twist bell's controls in a single row:
        Freq, Gain, Q, Speed, Move, Start knobs and the Shape menu.

        Speed is really two parameters (note lengths with Sync on, cycles per
        second with Sync off); only the one that applies is shown. */
    class TwistBellControls final : public juce::Component
    {
    public:
        TwistBellControls (juce::AudioProcessorValueTreeState& state, int bellIndex);

        void showSyncedSpeed (bool synced);
        void resized() override;

    private:
        static constexpr int shapeWidth = 130;
        static constexpr int labelHeight = 20;

        LabelledKnob freqKnob;
        LabelledKnob gainKnob;
        LabelledKnob qKnob;
        LabelledKnob syncSpeedKnob;
        LabelledKnob freeSpeedKnob;
        LabelledKnob moveKnob;
        LabelledKnob startKnob;

        juce::Label shapeLabel;
        juce::ComboBox shapeMenu;
        juce::AudioProcessorValueTreeState::ComboBoxAttachment shapeAttachment;
    };
}
