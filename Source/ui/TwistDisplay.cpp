#include "TwistDisplay.h"
#include "Parameters.h"
#include "TwistColours.h"
#include "TwistDisplayScale.h"

namespace rubbish
{
    namespace scale = twistDisplayScale;

    namespace
    {
        juce::RangedAudioParameter& findParameter (juce::AudioProcessorValueTreeState& state, const juce::String& id)
        {
            auto* parameter = state.getParameter (id);
            jassert (parameter != nullptr); // a parameter ID is missing from createParameterLayout()
            return *parameter;
        }

        const juce::Colour gridColour = juce::Colours::white.withAlpha (0.08f);
        const juce::Colour labelColour = juce::Colours::white.withAlpha (0.4f);
    }

    TwistDisplay::TwistDisplay (juce::AudioProcessorValueTreeState& state, TwistDisplaySource& positions)
        : source (positions),
          twistParameter (findParameter (state, ParamIDs::twist))
    {
        for (int bell = 0; bell < numBells; ++bell)
        {
            const auto ids = getTwistBellIDs (bell);
            bells[static_cast<size_t> (bell)] = { &findParameter (state, ids.freq), &findParameter (state, ids.gain),
                                                  &findParameter (state, ids.q),
                                                  &findParameter (state, getModAmountID (bell, bellFreqTarget (bell))) };
        }

        startTimerHz (refreshRateHz);
    }

    void TwistDisplay::setSelectedBell (int bellIndex)
    {
        selectedBell = juce::jlimit (0, numBells - 1, bellIndex);
        repaint();
    }

    float TwistDisplay::readValue (const juce::RangedAudioParameter& parameter)
    {
        return parameter.convertFrom0to1 (parameter.getValue());
    }

    void TwistDisplay::setValue (juce::RangedAudioParameter& parameter, float value)
    {
        parameter.setValueNotifyingHost (parameter.convertTo0to1 (value));
    }

    bool TwistDisplay::isTwistOn() const
    {
        return twistParameter.getValue() >= 0.5f;
    }

    TwistDisplay::BellView TwistDisplay::readBell (int bellIndex) const
    {
        const auto& p = bells[static_cast<size_t> (bellIndex)];
        const auto homeHz = readValue (*p.frequency);
        const auto published = source.getFrequency (bellIndex);
        const auto liveHz = isTwistOn() && published > 0.0f ? published : homeHz;

        return { homeHz, liveHz, readValue (*p.gain), twistQPercentToQ (readValue (*p.q)), readValue (*p.move) * 0.01f };
    }

    juce::Rectangle<float> TwistDisplay::getPlotArea() const
    {
        return getLocalBounds().toFloat().reduced (8.0f, 10.0f).withTrimmedBottom (14.0f);
    }

    void TwistDisplay::paint (juce::Graphics& g)
    {
        const auto bounds = getLocalBounds().toFloat();
        g.setColour (juce::Colours::black.withAlpha (0.35f));
        g.fillRoundedRectangle (bounds, 6.0f);

        const auto area = getPlotArea();
        paintGrid (g, area);

        std::array<BellView, numBells> views;
        for (int bell = 0; bell < numBells; ++bell)
            views[static_cast<size_t> (bell)] = readBell (bell);

        // While Twist is off, the bells are shown faintly so you can still set them up.
        const auto dimmed = ! isTwistOn();

        if (dimmed)
            g.beginTransparencyLayer (0.45f);

        for (int bell = 0; bell < numBells; ++bell)
        {
            paintRoamBar (g, area, bell, views[static_cast<size_t> (bell)]);
            paintBellCurve (g, area, bell, views[static_cast<size_t> (bell)]);
        }

        paintCombinedCurve (g, area, views);

        for (int bell = 0; bell < numBells; ++bell)
            paintHandles (g, area, bell, views[static_cast<size_t> (bell)]);

        if (dimmed)
            g.endTransparencyLayer();
    }

    void TwistDisplay::paintGrid (juce::Graphics& g, juce::Rectangle<float> area) const
    {
        const std::pair<float, const char*> marks[] { { 50.0f, "50" }, { 100.0f, "100" }, { 200.0f, "200" },
                                                      { 500.0f, "500" }, { 1000.0f, "1k" }, { 2000.0f, "2k" },
                                                      { 5000.0f, "5k" }, { 10000.0f, "10k" } };
        g.setFont (juce::FontOptions (11.0f));

        for (const auto& [hz, label] : marks)
        {
            const auto x = scale::frequencyToX (hz, area);
            g.setColour (gridColour);
            g.drawVerticalLine (juce::roundToInt (x), area.getY(), area.getBottom());
            g.setColour (labelColour);
            g.drawText (label, juce::Rectangle<float> (x - 20.0f, area.getBottom() + 1.0f, 40.0f, 13.0f),
                        juce::Justification::centred);
        }

        for (const auto db : { -12.0f, 0.0f, 12.0f })
        {
            g.setColour (db == 0.0f ? gridColour.withMultipliedAlpha (2.5f) : gridColour);
            g.drawHorizontalLine (juce::roundToInt (scale::gainToY (db, area)), area.getX(), area.getRight());
        }
    }

    void TwistDisplay::paintRoamBar (juce::Graphics& g, juce::Rectangle<float> area, int bellIndex,
                                     const BellView& bell) const
    {
        const auto octaves = std::abs (bell.move) * maxBellSweepOctaves;
        const auto left = scale::frequencyToX (juce::jmax (scale::minFrequencyHz, bell.homeHz * std::exp2 (-octaves)), area);
        const auto right = scale::frequencyToX (juce::jmin (scale::maxFrequencyHz, bell.homeHz * std::exp2 (octaves)), area);

        g.setColour (getTwistBellColour (bellIndex).withAlpha (0.3f));
        g.fillRoundedRectangle ({ left, area.getBottom() - 6.0f - 5.0f * static_cast<float> (bellIndex),
                                  juce::jmax (2.0f, right - left), 3.0f }, 1.5f);
    }

    void TwistDisplay::paintBellCurve (juce::Graphics& g, juce::Rectangle<float> area, int bellIndex,
                                       const BellView& bell) const
    {
        juce::Path curve;

        for (int i = 0; i <= curvePoints; ++i)
        {
            const auto x = area.getX() + area.getWidth() * static_cast<float> (i) / curvePoints;
            const auto db = scale::bellResponseDb (scale::xToFrequency (x, area), bell.liveHz, bell.gainDb, bell.q);
            const auto y = juce::jlimit (area.getY(), area.getBottom(), scale::gainToY (db, area));
            i == 0 ? curve.startNewSubPath (x, y) : curve.lineTo (x, y);
        }

        const auto alpha = bellIndex == selectedBell ? 0.75f : 0.4f;
        g.setColour (getTwistBellColour (bellIndex).withAlpha (alpha));
        g.strokePath (curve, juce::PathStrokeType (1.5f));
    }

    void TwistDisplay::paintCombinedCurve (juce::Graphics& g, juce::Rectangle<float> area,
                                           const std::array<BellView, numBells>& views) const
    {
        juce::Path curve;

        for (int i = 0; i <= curvePoints; ++i)
        {
            const auto x = area.getX() + area.getWidth() * static_cast<float> (i) / curvePoints;
            const auto hz = scale::xToFrequency (x, area);
            float db = 0.0f;

            for (const auto& bell : views)
                db += scale::bellResponseDb (hz, bell.liveHz, bell.gainDb, bell.q);

            const auto y = juce::jlimit (area.getY(), area.getBottom(), scale::gainToY (db, area));
            i == 0 ? curve.startNewSubPath (x, y) : curve.lineTo (x, y);
        }

        g.setColour (juce::Colours::white.withAlpha (0.9f));
        g.strokePath (curve, juce::PathStrokeType (2.5f, juce::PathStrokeType::curved));
    }

    void TwistDisplay::paintHandles (juce::Graphics& g, juce::Rectangle<float> area, int bellIndex,
                                     const BellView& bell) const
    {
        const auto colour = getTwistBellColour (bellIndex);
        const auto y = scale::gainToY (bell.gainDb, area);
        const auto radius = bellIndex == selectedBell ? 7.0f : 5.5f;

        // Hollow ring: home position. Solid dot: where the bell is right now.
        g.setColour (colour);
        g.drawEllipse (juce::Rectangle<float> (radius * 2.0f, radius * 2.0f)
                           .withCentre ({ scale::frequencyToX (bell.homeHz, area), y }), 1.5f);
        g.fillEllipse (juce::Rectangle<float> (radius * 2.0f, radius * 2.0f)
                           .withCentre ({ scale::frequencyToX (bell.liveHz, area), y }));
    }

    int TwistDisplay::findBellNear (juce::Point<float> position) const
    {
        const auto area = getPlotArea();
        auto nearest = -1;
        auto nearestDistance = grabRadius;

        for (int bell = 0; bell < numBells; ++bell)
        {
            const auto view = readBell (bell);
            const auto y = scale::gainToY (view.gainDb, area);

            for (const auto hz : { view.liveHz, view.homeHz })
            {
                const auto distance = position.getDistanceFrom ({ scale::frequencyToX (hz, area), y });

                if (distance < nearestDistance)
                {
                    nearest = bell;
                    nearestDistance = distance;
                }
            }
        }

        return nearest;
    }

    void TwistDisplay::mouseDown (const juce::MouseEvent& event)
    {
        draggedBell = findBellNear (event.position);

        if (draggedBell < 0)
            return;

        setSelectedBell (draggedBell);

        if (onBellPicked)
            onBellPicked (draggedBell);

        const auto& p = bells[static_cast<size_t> (draggedBell)];
        p.frequency->beginChangeGesture();
        p.gain->beginChangeGesture();
    }

    // Dragging sets the bell's home position; its LFO keeps moving it around that point.
    void TwistDisplay::mouseDrag (const juce::MouseEvent& event)
    {
        if (draggedBell < 0)
            return;

        const auto area = getPlotArea();
        const auto& p = bells[static_cast<size_t> (draggedBell)];
        setValue (*p.frequency, scale::xToFrequency (event.position.x, area));
        setValue (*p.gain, scale::yToGain (event.position.y, area));
    }

    void TwistDisplay::mouseUp (const juce::MouseEvent&)
    {
        if (draggedBell < 0)
            return;

        const auto& p = bells[static_cast<size_t> (draggedBell)];
        p.frequency->endChangeGesture();
        p.gain->endChangeGesture();
        draggedBell = -1;
    }

    // Scrolling over a bell makes it narrower (up) or wider (down).
    void TwistDisplay::mouseWheelMove (const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
    {
        const auto bell = findBellNear (event.position);

        if (bell < 0)
            return;

        auto& q = *bells[static_cast<size_t> (bell)].q;
        const auto step = wheel.isReversed ? -wheel.deltaY : wheel.deltaY;

        constexpr float percentPerScroll = 40.0f; // one wheel notch moves Q by roughly 4%

        q.beginChangeGesture();
        setValue (q, juce::jlimit (0.0f, 100.0f, readValue (q) + step * percentPerScroll));
        q.endChangeGesture();
    }
}
