#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace rubbish::panelStyle
{
    inline constexpr int titleHeight = 28;
    inline constexpr int padding = 10;

    /** Draws the outlined box and title shared by every panel. */
    void drawFrame (juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title);

    /** The area inside a panel's frame, below its title. */
    juce::Rectangle<int> getContentArea (juce::Rectangle<int> bounds);
}
