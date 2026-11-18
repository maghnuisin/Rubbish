#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace rubbish
{
    /** The tall ↔ button between the Drive and Filters panels. Each click swaps the
        Filter Order (Clean first <-> Scream); the panels then trade places. */
    class FilterOrderSwapButton final : public juce::Button
    {
    public:
        explicit FilterOrderSwapButton (juce::RangedAudioParameter& filterOrderParameter);

        void paintButton (juce::Graphics& g, bool isHighlighted, bool isDown) override;

        static constexpr int preferredWidth = 40;
        static constexpr int preferredHeight = 130;

    private:
        void clicked() override;
        void showOrder (float orderIndex);
        static void drawDoubleArrow (juce::Graphics& g, juce::Rectangle<float> area);

        bool screamOrder = false;
        juce::ParameterAttachment attachment;
    };
}
