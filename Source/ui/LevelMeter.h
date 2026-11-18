#pragma once

#include "dsp/LevelMeterSource.h"

#include <juce_gui_basics/juce_gui_basics.h>

namespace rubbish
{
    /** A vertical peak meter from -60 to 0 dBFS, with the recent peak shown as a
        number on top (held for a moment so it can be read) and a caption below. */
    class LevelMeter final : public juce::Component,
                             private juce::Timer
    {
    public:
        LevelMeter (LevelMeterSource& levelSource, const juce::String& captionText);

        void paint (juce::Graphics& g) override;

        static constexpr int preferredWidth = 56;

    private:
        void timerCallback() override;
        void updateBar (float peakDb) noexcept;
        void updateReadout (float peakDb) noexcept;

        void paintBar (juce::Graphics& g, juce::Rectangle<float> area) const;
        void paintText (juce::Graphics& g, juce::Rectangle<int> area, const juce::String& text) const;
        float levelToProportion (float decibels) const noexcept;
        juce::String formatReadout() const;

        static constexpr int refreshRateHz = 30;
        static constexpr int readoutHoldRefreshes = 45;  // 1.5 seconds
        static constexpr float floorDb = -60.0f;
        static constexpr float fallDbPerRefresh = 1.5f;  // about 45 dB per second
        static constexpr int textHeight = 18;
        static constexpr int barWidth = 18;

        LevelMeterSource& source;
        const juce::String caption;
        float barDb = floorDb;
        float readoutDb = floorDb;
        int readoutHoldRemaining = 0;
    };
}
