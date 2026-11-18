#pragma once

#include "LabelledKnob.h"

namespace rubbish
{
    /** Low Cut and High Cut, each with its Peak knob beside it, and Filter Follow.
        (Filter Order is the swap button between the panels.) */
    class FiltersPanel final : public juce::Component
    {
    public:
        explicit FiltersPanel (juce::AudioProcessorValueTreeState& state);

        void paint (juce::Graphics& g) override;
        void resized() override;

    private:
        static void placeKnobPair (juce::Rectangle<int> row, LabelledKnob& cutoff, LabelledKnob& peak);

        LabelledKnob lowCutKnob;
        LabelledKnob lowPeakKnob;
        LabelledKnob highCutKnob;
        LabelledKnob highPeakKnob;
        LabelledKnob filterFollowKnob;
    };
}
