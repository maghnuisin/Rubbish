#include "SafetyLimiter.h"
#include "BlockHelpers.h"

#include <cmath>

namespace rubbish
{
    namespace
    {
        // -0.3 dBFS: a little headroom so conversion to the host's format can't tip it over.
        constexpr float ceiling = 0.966f;
    }

    void SafetyLimiter::prepare (double sampleRate)
    {
        jassert (sampleRate > 0.0);
        releaseMultiplier = static_cast<float> (std::exp (-1.0 / (releaseSeconds * sampleRate)));
        reset();
    }

    void SafetyLimiter::reset()
    {
        gainReduction = 0.0f;
    }

    void SafetyLimiter::process (juce::dsp::AudioBlock<float>& block) noexcept
    {
        for (size_t i = 0; i < block.getNumSamples(); ++i)
        {
            // Both channels share one gain so the stereo image doesn't shift when limiting.
            const auto sampleGain = nextGain (loudestSampleAt (block, i));

            for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
            {
                auto& sample = block.getChannelPointer (ch)[i];
                sample = juce::jlimit (-ceiling, ceiling, sample * sampleGain);
            }
        }
    }

    // Tracks how far below unity the gain is, rather than the gain itself: the
    // reduction can shrink all the way to zero, whereas a gain creeping up towards
    // 1.0 gets stuck just short of it through floating-point rounding.
    float SafetyLimiter::nextGain (float peak) noexcept
    {
        const auto reductionNeeded = peak > ceiling ? 1.0f - ceiling / peak : 0.0f;
        const auto released = gainReduction * releaseMultiplier;
        gainReduction = juce::jmax (reductionNeeded, released);
        return 1.0f - gainReduction;
    }
}
