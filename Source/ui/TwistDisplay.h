#pragma once

#include "dsp/TwistDisplaySource.h"

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <array>
#include <functional>

namespace rubbish
{
    /** The moving Twist display: each bell's curve and the combined EQ curve,
        live dots where the bells are now, hollow rings at their home positions,
        and a faint bar showing how far each one can roam.

        Drag a dot or ring: left/right = Freq, up/down = Gain. Scroll over it = Q. */
    class TwistDisplay final : public juce::Component,
                               private juce::Timer
    {
    public:
        TwistDisplay (juce::AudioProcessorValueTreeState& state, TwistDisplaySource& positions);

        /** Called when a dot is grabbed, so the tabs can follow. */
        std::function<void (int bellIndex)> onBellPicked;

        void setSelectedBell (int bellIndex);

        void paint (juce::Graphics& g) override;
        void mouseDown (const juce::MouseEvent& event) override;
        void mouseDrag (const juce::MouseEvent& event) override;
        void mouseUp (const juce::MouseEvent& event) override;
        void mouseWheelMove (const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

        static constexpr int numBells = TwistDisplaySource::numBells;

    private:
        struct BellParameters
        {
            juce::RangedAudioParameter* frequency;
            juce::RangedAudioParameter* gain;
            juce::RangedAudioParameter* q;
            juce::RangedAudioParameter* move;
        };

        struct BellView
        {
            float homeHz, liveHz, gainDb, q, move;
        };

        void timerCallback() override { repaint(); }

        juce::Rectangle<float> getPlotArea() const;
        bool isTwistOn() const;
        BellView readBell (int bellIndex) const;
        int findBellNear (juce::Point<float> position) const;

        void paintGrid (juce::Graphics& g, juce::Rectangle<float> area) const;
        void paintRoamBar (juce::Graphics& g, juce::Rectangle<float> area, int bellIndex, const BellView& bell) const;
        void paintBellCurve (juce::Graphics& g, juce::Rectangle<float> area, int bellIndex, const BellView& bell) const;
        void paintCombinedCurve (juce::Graphics& g, juce::Rectangle<float> area,
                                 const std::array<BellView, numBells>& bells) const;
        void paintHandles (juce::Graphics& g, juce::Rectangle<float> area, int bellIndex, const BellView& bell) const;

        static float readValue (const juce::RangedAudioParameter& parameter);
        static void setValue (juce::RangedAudioParameter& parameter, float value);

        static constexpr int refreshRateHz = 30;
        static constexpr float grabRadius = 14.0f;
        static constexpr int curvePoints = 240;

        TwistDisplaySource& source;
        juce::RangedAudioParameter& twistParameter;
        std::array<BellParameters, numBells> bells;
        int selectedBell = 0;
        int draggedBell = -1;
    };
}
