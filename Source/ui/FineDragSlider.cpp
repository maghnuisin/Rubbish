#include "FineDragSlider.h"

namespace rubbish
{
    FineDragSlider::FineDragSlider()
    {
        // Stop JUCE's own modifier behaviour (switching to velocity drag) from
        // competing with fine adjustment.
        setVelocityModeParameters (1.0, 1, 0.0, false);
    }

    bool FineDragSlider::wantsFineAdjustment (const juce::ModifierKeys& mods) noexcept
    {
        return mods.isShiftDown() || mods.isCommandDown() || mods.isCtrlDown();
    }

    void FineDragSlider::mouseDown (const juce::MouseEvent& e)
    {
        virtualPosition = lastRealPosition = e.position;
        juce::Slider::mouseDown (e);
    }

    void FineDragSlider::mouseDrag (const juce::MouseEvent& e)
    {
        const auto scale = wantsFineAdjustment (e.mods) ? fineScale : 1.0f;
        virtualPosition += (e.position - lastRealPosition) * scale;
        lastRealPosition = e.position;

        juce::Slider::mouseDrag (e.withNewPosition (virtualPosition));
    }
}
