#include "TwistBellControls.h"
#include "Parameters.h"
#include "TwistColours.h"

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
        : freqKnob (state, getTwistBellIDs (bellIndex).freq),
          gainKnob (state, getTwistBellIDs (bellIndex).gain),
          qKnob (state, getTwistBellIDs (bellIndex).q),
          syncSpeedKnob (state, getTwistBellIDs (bellIndex).speedSync),
          freeSpeedKnob (state, getTwistBellIDs (bellIndex).speedFree),
          moveKnob (state, getTwistBellIDs (bellIndex).move),
          startKnob (state, getTwistBellIDs (bellIndex).start),
          shapeAttachment (state, getTwistBellIDs (bellIndex).shape, withShapeItems (shapeMenu))
    {
        nameLabel.setText ("Bell " + juce::String (bellIndex + 1), juce::dontSendNotification);
        nameLabel.setColour (juce::Label::textColourId, getTwistBellColour (bellIndex));
        nameLabel.setFont (juce::FontOptions (15.0f, juce::Font::bold));
        addAndMakeVisible (nameLabel);

        for (auto* knob : { &freqKnob, &gainKnob, &qKnob, &syncSpeedKnob, &freeSpeedKnob, &moveKnob, &startKnob })
        {
            knob->useRowStyle();
            addAndMakeVisible (knob);
        }

        addAndMakeVisible (shapeMenu);
        showSyncedSpeed (true);
    }

    const juce::StringArray& TwistBellControls::getColumnNames()
    {
        static const juce::StringArray names { "", "Freq", "Gain", "Q", "Speed", "Move", "Start", "Shape" };
        return names;
    }

    std::array<juce::Rectangle<int>, TwistBellControls::numColumns> TwistBellControls::getColumns (juce::Rectangle<int> row)
    {
        std::array<juce::Rectangle<int>, numColumns> columns;
        columns[0] = row.removeFromLeft (nameWidth);
        columns[numColumns - 1] = row.removeFromRight (shapeWidth);

        const auto knobWidth = row.getWidth() / (numColumns - 2);
        for (size_t i = 1; i < numColumns - 1; ++i)
            columns[i] = i == numColumns - 2 ? row : row.removeFromLeft (knobWidth);

        return columns;
    }

    void TwistBellControls::showSyncedSpeed (bool synced)
    {
        syncSpeedKnob.setVisible (synced);
        freeSpeedKnob.setVisible (! synced);
    }

    void TwistBellControls::resized()
    {
        const auto columns = getColumns (getLocalBounds());
        nameLabel.setBounds (columns[0]);

        LabelledKnob* knobs[] { &freqKnob, &gainKnob, &qKnob, &syncSpeedKnob, &moveKnob, &startKnob };
        for (size_t i = 0; i < std::size (knobs); ++i)
            knobs[i]->setBounds (columns[i + 1].reduced (4, 0));

        freeSpeedKnob.setBounds (syncSpeedKnob.getBounds());
        shapeMenu.setBounds (columns[numColumns - 1].withSizeKeepingCentre (columns[numColumns - 1].getWidth() - 8, 26));
    }
}
