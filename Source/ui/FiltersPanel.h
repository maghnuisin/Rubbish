#pragma once

#include "LabelledKnob.h"

namespace rubbish
{
    /** One row: Low Cut and its Peak, High Cut and its Peak, then Filter Follow.
        (Filter Order is the swap button between the panels.) */
    class FiltersPanel final : public juce::Component
    {
    public:
        explicit FiltersPanel (juce::AudioProcessorValueTreeState& state);

        void paint (juce::Graphics& g) override;
        void resized() override;

        static constexpr int preferredWidth = 380;

    private:

        LabelledKnob lowCutKnob;
        LabelledKnob lowPeakKnob;
        LabelledKnob highCutKnob;
        LabelledKnob highPeakKnob;
        LabelledKnob filterFollowKnob;
    };
}
