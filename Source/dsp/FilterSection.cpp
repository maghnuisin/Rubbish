#include "FilterSection.h"

namespace rubbish
{
    void FilterSection::prepare (const juce::dsp::ProcessSpec& spec)
    {
        lowCut.prepare (spec);
        highCut.prepare (spec);
    }

    void FilterSection::reset()
    {
        lowCut.reset();
        highCut.reset();
    }

    void FilterSection::setLowCut (float cutoffHz, float peak) noexcept
    {
        lowCut.setCutoffHz (cutoffHz);
        lowCut.setPeak (peak);
    }

    void FilterSection::setHighCut (float cutoffHz, float peak) noexcept
    {
        highCut.setCutoffHz (cutoffHz);
        highCut.setPeak (peak);
    }

    void FilterSection::process (juce::dsp::AudioBlock<float>& block, const float* highCutMultipliers) noexcept
    {
        lowCut.process (block);
        highCut.process (block, highCutMultipliers);
    }
}
