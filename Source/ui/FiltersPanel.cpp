#include "FiltersPanel.h"
#include "PanelStyle.h"
#include "Parameters.h"

namespace rubbish
{
    FiltersPanel::FiltersPanel (juce::AudioProcessorValueTreeState& state)
        : lowCutKnob (state, ParamIDs::lowCut),
          lowPeakKnob (state, ParamIDs::lowPeak),
          highCutKnob (state, ParamIDs::highCut),
          highPeakKnob (state, ParamIDs::highPeak),
          filterFollowKnob (state, ParamIDs::filterFollow)
    {
        for (auto* child : std::initializer_list<juce::Component*> { &lowCutKnob, &lowPeakKnob, &highCutKnob,
                                                                     &highPeakKnob, &filterFollowKnob })
            addAndMakeVisible (child);
    }

    void FiltersPanel::paint (juce::Graphics& g)
    {
        panelStyle::drawFrame (g, getLocalBounds(), "Filters");
    }

    void FiltersPanel::resized()
    {
        auto area = panelStyle::getContentArea (getLocalBounds());

        const auto rowHeight = area.getHeight() / 3;
        placeKnobPair (area.removeFromTop (rowHeight), lowCutKnob, lowPeakKnob);
        placeKnobPair (area.removeFromTop (rowHeight), highCutKnob, highPeakKnob);
        filterFollowKnob.setBounds (area.withSizeKeepingCentre (area.getWidth() / 2, area.getHeight()).reduced (4));
    }

    void FiltersPanel::placeKnobPair (juce::Rectangle<int> row, LabelledKnob& cutoff, LabelledKnob& peak)
    {
        cutoff.setBounds (row.removeFromLeft (row.getWidth() / 2).reduced (4));
        peak.setBounds (row.reduced (4));
    }
}
