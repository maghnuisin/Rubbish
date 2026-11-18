#pragma once

#include <juce_dsp/juce_dsp.h>

#include <vector>

namespace rubbish
{
    /** One EQ bell (boost or cut) whose frequency can change every sample.

        It is a trapezoidal state-variable filter (Andrew Simper's design), which stays
        stable and click-free however fast its settings move, at any gain or Q. */
    class TwistBell
    {
    public:
        void prepare (const juce::dsp::ProcessSpec& spec);
        void reset() noexcept;

        void setShape (float frequencyHz, float gainDb, float q) noexcept;
        float processSample (size_t channel, float input) noexcept;

        float getMaxFrequencyHz() const noexcept { return maxFrequencyHz; }

    private:
        double sampleRate = 44100.0;
        float maxFrequencyHz = 20000.0f;

        float a1 = 1.0f, a2 = 0.0f, a3 = 0.0f, bandGain = 0.0f;

        std::vector<float> state1;
        std::vector<float> state2;
    };
}
