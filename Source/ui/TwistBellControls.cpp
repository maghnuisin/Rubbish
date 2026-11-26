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
          syncSpeedKnob (state, getLfoIDs (bellIndex).speedSync),
          freeSpeedKnob (state, getLfoIDs (bellIndex).speedFree),
          moveKnob (state, getModAmountID (bellIndex, bellFreqTarget (bellIndex))),
          startKnob (state, getLfoIDs (bellIndex).start),
          shapeAttachment (state, getLfoIDs (bellIndex).shape, withShapeItems (shapeMenu)),
          syncAttachment (state, getLfoIDs (bellIndex).sync, syncButton),
          syncWatcher (*state.getParameter (getLfoIDs (bellIndex).sync), [this] (float value) { showSyncedSpeed (value >= 0.5f); })
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
        syncButton.setClickingTogglesState (true);
        addAndMakeVisible (syncButton);
        syncWatcher.sendInitialUpdate();
    }

    const juce::StringArray& TwistBellControls::getColumnNames()
    {
        static const juce::StringArray names { "", "Freq", "Gain", "Q", "Speed", "Move", "Start", "Shape", "" };
        return names;
    }

    std::array<juce::Rectangle<int>, TwistBellControls::numColumns> TwistBellControls::getColumns (juce::Rectangle<int> row)
    {
        std::array<juce::Rectangle<int>, numColumns> columns;
        columns[0] = row.removeFromLeft (nameWidth);
        columns[numColumns - 1] = row.removeFromRight (syncWidth);
        columns[numColumns - 2] = row.removeFromRight (shapeWidth);

        constexpr size_t numKnobs = numColumns - 3;
        const auto knobWidth = row.getWidth() / static_cast<int> (numKnobs);
        for (size_t i = 1; i <= numKnobs; ++i)
            columns[i] = i == numKnobs ? row : row.removeFromLeft (knobWidth);

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
        shapeMenu.setBounds (columns[numColumns - 2].withSizeKeepingCentre (columns[numColumns - 2].getWidth() - 8, 26));
        syncButton.setBounds (columns[numColumns - 1].withSizeKeepingCentre (columns[numColumns - 1].getWidth() - 8, 26));
    }
}
