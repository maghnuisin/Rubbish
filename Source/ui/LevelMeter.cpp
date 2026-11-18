#include "LevelMeter.h"

namespace rubbish
{
    LevelMeter::LevelMeter (LevelMeterSource& levelSource, const juce::String& captionText)
        : source (levelSource),
          caption (captionText)
    {
        startTimerHz (refreshRateHz);
    }

    void LevelMeter::timerCallback()
    {
        const auto previousBar = barDb;
        const auto previousReadout = formatReadout();

        const auto peakDb = juce::Decibels::gainToDecibels (source.takePeak(), floorDb);
        updateBar (peakDb);
        updateReadout (peakDb);

        if (! juce::approximatelyEqual (barDb, previousBar) || formatReadout() != previousReadout)
            repaint();
    }

    // The bar jumps up immediately to show peaks, then falls slowly so it's readable.
    void LevelMeter::updateBar (float peakDb) noexcept
    {
        barDb = juce::jmax (peakDb, juce::jmax (floorDb, barDb - fallDbPerRefresh));
    }

    // The number holds the highest recent peak long enough to read, then follows the bar.
    void LevelMeter::updateReadout (float peakDb) noexcept
    {
        if (peakDb >= readoutDb)
        {
            readoutDb = peakDb;
            readoutHoldRemaining = readoutHoldRefreshes;
        }
        else if (readoutHoldRemaining > 0)
        {
            --readoutHoldRemaining;
        }
        else
        {
            readoutDb = barDb;
        }
    }

    juce::String LevelMeter::formatReadout() const
    {
        if (readoutDb <= floorDb)
            return "-inf dB";

        return juce::String (readoutDb, 1) + " dB";
    }

    float LevelMeter::levelToProportion (float decibels) const noexcept
    {
        return juce::jlimit (0.0f, 1.0f, (decibels - floorDb) / -floorDb);
    }

    void LevelMeter::paint (juce::Graphics& g)
    {
        auto area = getLocalBounds();
        paintText (g, area.removeFromTop (textHeight), formatReadout());
        paintText (g, area.removeFromBottom (textHeight), caption);

        const auto barArea = area.reduced (0, 4).withSizeKeepingCentre (barWidth, area.getHeight() - 8);
        paintBar (g, barArea.toFloat());
    }

    void LevelMeter::paintText (juce::Graphics& g, juce::Rectangle<int> area, const juce::String& text) const
    {
        g.setColour (juce::Colours::white);
        g.setFont (juce::FontOptions (12.0f));
        g.drawText (text, area, juce::Justification::centred, false);
    }

    void LevelMeter::paintBar (juce::Graphics& g, juce::Rectangle<float> area) const
    {
        g.setColour (juce::Colours::black.withAlpha (0.5f));
        g.fillRoundedRectangle (area, 3.0f);

        const auto filled = area.withTop (area.getBottom() - area.getHeight() * levelToProportion (barDb));
        const auto nearClipping = barDb > -1.0f;
        g.setColour (nearClipping ? juce::Colours::orange : juce::Colours::limegreen);
        g.fillRoundedRectangle (filled.reduced (2.0f), 2.0f);
    }
}
