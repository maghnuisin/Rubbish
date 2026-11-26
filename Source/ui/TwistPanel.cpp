#include "TwistPanel.h"
#include "PanelStyle.h"
#include "Parameters.h"

namespace rubbish
{
    TwistPanel::TwistPanel (juce::AudioProcessorValueTreeState& state, TwistDisplaySource& bellPositions)
        : display (state, bellPositions),
          twistAttachment (state, ParamIDs::twist, twistButton)
    {
        twistButton.setClickingTogglesState (true);
        addAndMakeVisible (display);
        addAndMakeVisible (twistButton);

        createColumnNames();

        for (int bell = 0; bell < numBells; ++bell)
        {
            bellRows[static_cast<size_t> (bell)] = std::make_unique<TwistBellControls> (state, bell);
            addAndMakeVisible (*bellRows[static_cast<size_t> (bell)]);
        }

    }

    void TwistPanel::createColumnNames()
    {
        for (const auto& name : TwistBellControls::getColumnNames())
        {
            auto* label = columnNames.add (std::make_unique<juce::Label> (juce::String(), name));
            label->setJustificationType (juce::Justification::centred);
            label->setFont (juce::FontOptions (13.0f));
            label->setColour (juce::Label::textColourId, juce::Colours::white.withAlpha (0.7f));
            addAndMakeVisible (label);
        }
    }

    void TwistPanel::paint (juce::Graphics& g)
    {
        panelStyle::drawFrame (g, getLocalBounds(), "Twist");
    }

    void TwistPanel::resized()
    {
        auto area = panelStyle::getContentArea (getLocalBounds());
        layOutControls (area.removeFromBottom (controlsHeight));
        area.removeFromBottom (6);
        display.setBounds (area);
    }

    void TwistPanel::layOutControls (juce::Rectangle<int> area)
    {
        twistButton.setBounds (area.removeFromLeft (toggleWidth).withSizeKeepingCentre (toggleWidth, 30));
        area.removeFromLeft (panelStyle::padding);

        const auto columns = TwistBellControls::getColumns (area.removeFromTop (headerHeight));
        for (size_t i = 0; i < columns.size(); ++i)
            columnNames[static_cast<int> (i)]->setBounds (columns[i]);

        for (auto& row : bellRows)
            row->setBounds (area.removeFromTop (rowHeight));
    }
}
