#include "TwistBellControls.h"
#include "Parameters.h"

namespace rubbish
{
    namespace
    {
        // The menu needs its items before the attachment connects it to the parameter.
        // ComboBox item IDs must not be zero, so they start at 1.
        juce::ComboBox& withShapeItems (juce::ComboBox& menu)
        {
            menu.addItemList (twistShapeNames, 1);
            return menu;
        }
    }

    TwistBellControls::TwistBellControls (juce::AudioProcessorValueTreeState& state, int bellIndex)
        : freqKnob (state, getTwistBellIDs (bellIndex).freq, "Freq"),
          gainKnob (state, getTwistBellIDs (bellIndex).gain, "Gain"),
          qKnob (state, getTwistBellIDs (bellIndex).q, "Q"),
          syncSpeedKnob (state, getTwistBellIDs (bellIndex).speedSync, "Speed"),
          freeSpeedKnob (state, getTwistBellIDs (bellIndex).speedFree, "Speed"),
          moveKnob (state, getTwistBellIDs (bellIndex).move, "Move"),
          startKnob (state, getTwistBellIDs (bellIndex).start, "Start"),
          shapeAttachment (state, getTwistBellIDs (bellIndex).shape, withShapeItems (shapeMenu))
    {
        shapeLabel.setText ("Shape", juce::dontSendNotification);
        shapeLabel.setJustificationType (juce::Justification::centred);

        for (auto* child : std::initializer_list<juce::Component*> { &freqKnob, &gainKnob, &qKnob, &syncSpeedKnob,
                                                                     &freeSpeedKnob, &moveKnob, &startKnob,
                                                                     &shapeLabel, &shapeMenu })
            addAndMakeVisible (child);

        showSyncedSpeed (true);
    }

    void TwistBellControls::showSyncedSpeed (bool synced)
    {
        syncSpeedKnob.setVisible (synced);
        freeSpeedKnob.setVisible (! synced);
    }

    void TwistBellControls::resized()
    {
        auto area = getLocalBounds();

        auto shapeArea = area.removeFromRight (shapeWidth).reduced (6, 0);
        shapeLabel.setBounds (shapeArea.removeFromTop (labelHeight));
        shapeMenu.setBounds (shapeArea.removeFromTop (28));

        const auto knobWidth = area.getWidth() / 6;
        freqKnob.setBounds (area.removeFromLeft (knobWidth));
        gainKnob.setBounds (area.removeFromLeft (knobWidth));
        qKnob.setBounds (area.removeFromLeft (knobWidth));

        const auto speedArea = area.removeFromLeft (knobWidth);
        syncSpeedKnob.setBounds (speedArea);
        freeSpeedKnob.setBounds (speedArea);

        moveKnob.setBounds (area.removeFromLeft (knobWidth));
        startKnob.setBounds (area);
    }
}
