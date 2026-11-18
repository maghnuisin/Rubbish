#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace rubbish
{
    /** A knob that moves ten times more slowly while Shift, Cmd or Ctrl is held,
        for fine adjustment. Pressing or releasing the key mid-drag never makes
        the value jump. Everything else is the standard JUCE slider. */
    class FineDragSlider final : public juce::Slider
    {
    public:
        FineDragSlider();

        void mouseDown (const juce::MouseEvent& e) override;
        void mouseDrag (const juce::MouseEvent& e) override;

    private:
        static bool wantsFineAdjustment (const juce::ModifierKeys& mods) noexcept;

        static constexpr float fineScale = 0.1f;

        // The slider is fed a "virtual" mouse position that moves by the real
        // movement times the current scale, so switching scale mid-drag is seamless.
        juce::Point<float> virtualPosition;
        juce::Point<float> lastRealPosition;
    };
}
