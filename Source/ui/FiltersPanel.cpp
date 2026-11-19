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
        const auto knobWidth = area.getWidth() / 5;

        for (auto* knob : { &lowCutKnob, &lowPeakKnob, &highCutKnob, &highPeakKnob })
            knob->setBounds (area.removeFromLeft (knobWidth).reduced (3, 0));

        filterFollowKnob.setBounds (area.reduced (3, 0));
    }
}
