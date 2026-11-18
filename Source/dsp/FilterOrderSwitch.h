#pragma once

#include <juce_dsp/juce_dsp.h>

namespace rubbish
{
    /** Order of the distortion and filters. Matches the Filter Order parameter choices. */
    enum class FilterOrder
    {
        cleanFirst = 0, // distortion, then filters
        scream          // filters, then distortion
    };

    /** Swaps the processing order without a click: the processed signal dips
        briefly to silence, the order changes, then it fades back in. */
    class FilterOrderSwitch
    {
    public:
        void prepare (double sampleRate);
        void reset (FilterOrder order);

        /** Call once at the start of each block, before reading getActiveOrder(). */
        void setRequestedOrder (FilterOrder requested) noexcept;
        FilterOrder getActiveOrder() const noexcept { return active; }

        void applyFade (juce::dsp::AudioBlock<float>& block) noexcept;

    private:
        bool isSilent() const noexcept;

        static constexpr double fadeSeconds = 0.01;

        FilterOrder active = FilterOrder::cleanFirst;
        juce::SmoothedValue<float> fadeGain { 1.0f };
    };
}
