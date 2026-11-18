#include "FilterOrderSwapButton.h"

namespace rubbish
{
    FilterOrderSwapButton::FilterOrderSwapButton (juce::RangedAudioParameter& filterOrderParameter)
        : juce::Button ("Swap Filter Order"),
          attachment (filterOrderParameter, [this] (float orderIndex) { showOrder (orderIndex); })
    {
        attachment.sendInitialUpdate();
    }

    void FilterOrderSwapButton::clicked()
    {
        attachment.setValueAsCompleteGesture (screamOrder ? 0.0f : 1.0f);
    }

    void FilterOrderSwapButton::showOrder (float orderIndex)
    {
        screamOrder = juce::roundToInt (orderIndex) == 1;
        setTooltip (screamOrder ? "Filter Order: Scream (filters, then distortion). Click to swap."
                                : "Filter Order: Clean first (distortion, then filters). Click to swap.");
        repaint();
    }

    void FilterOrderSwapButton::paintButton (juce::Graphics& g, bool isHighlighted, bool isDown)
    {
        const auto bounds = getLocalBounds().toFloat().reduced (1.0f);
        const auto corner = bounds.getWidth() * 0.5f;

        auto fill = juce::Colours::white.withAlpha (0.08f);
        if (isHighlighted) fill = fill.brighter().withAlpha (0.16f);
        if (isDown)        fill = juce::Colours::white.withAlpha (0.24f);

        g.setColour (fill);
        g.fillRoundedRectangle (bounds, corner);
        g.setColour (juce::Colours::white.withAlpha (0.25f));
        g.drawRoundedRectangle (bounds, corner, 1.0f);

        g.setColour (juce::Colours::white);
        drawDoubleArrow (g, bounds.withSizeKeepingCentre (bounds.getWidth() * 0.7f, 16.0f));
    }

    void FilterOrderSwapButton::drawDoubleArrow (juce::Graphics& g, juce::Rectangle<float> area)
    {
        const auto centre = area.getCentre();
        constexpr float thickness = 2.5f, headWidth = 10.0f, headLength = 8.0f;

        g.drawArrow ({ centre, { area.getRight(), centre.y } }, thickness, headWidth, headLength);
        g.drawArrow ({ centre, { area.getX(), centre.y } }, thickness, headWidth, headLength);
    }
}
