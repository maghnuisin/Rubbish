#include "PanelStyle.h"

namespace rubbish::panelStyle
{
    void drawFrame (juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title)
    {
        const auto frame = bounds.toFloat().reduced (1.0f);
        g.setColour (juce::Colours::white.withAlpha (0.25f));
        g.drawRoundedRectangle (frame, 6.0f, 1.0f);

        g.setColour (juce::Colours::white);
        g.setFont (juce::FontOptions (17.0f, juce::Font::bold));
        g.drawText (title, bounds.removeFromTop (titleHeight), juce::Justification::centred);
    }

    juce::Rectangle<int> getContentArea (juce::Rectangle<int> bounds)
    {
        bounds.removeFromTop (titleHeight);
        return bounds.reduced (padding);
    }
}
