#pragma once

#include <juce_graphics/juce_graphics.h>

namespace rubbish
{
    /** Each Twist bell's colour, shared by the tabs and the display.
        Coral, teal and purple: easy to tell apart on the dark background. */
    inline juce::Colour getTwistBellColour (int bellIndex)
    {
        const juce::Colour colours[] { juce::Colour (0xffd85a30), juce::Colour (0xff1d9e75), juce::Colour (0xff7f77dd) };
        return colours[juce::jlimit (0, 2, bellIndex)];
    }
}
