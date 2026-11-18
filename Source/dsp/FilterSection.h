#pragma once

#include "ResonantFilter.h"

namespace rubbish
{
    /** The Low Cut (resonant high-pass) followed by the High Cut (resonant low-pass). */
    class FilterSection
    {
    public:
        void prepare (const juce::dsp::ProcessSpec& spec);
        void reset();

        void setLowCut (float cutoffHz, float peak) noexcept;
        void setHighCut (float cutoffHz, float peak) noexcept;

        /** highCutMultipliers (optional, one per sample) move the High Cut for Filter Follow. */
        void process (juce::dsp::AudioBlock<float>& block, const float* highCutMultipliers = nullptr) noexcept;

    private:
        ResonantFilter lowCut { FilterType::highPass };
        ResonantFilter highCut { FilterType::lowPass };
    };
}
