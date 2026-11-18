#include "LoudnessTracker.h"
#include "BlockHelpers.h"

namespace rubbish
{
    void LoudnessTracker::configure (juce::dsp::BallisticsFilter<float>& follower, double sampleRate)
    {
        follower.prepare ({ sampleRate, 1, 1 });
        follower.setLevelCalculationType (juce::dsp::BallisticsFilterLevelCalculationType::peak);
        follower.setAttackTime (attackMs);
        follower.setReleaseTime (releaseMs);
    }

    void LoudnessTracker::prepare (const juce::dsp::ProcessSpec& spec)
    {
        jassert (spec.maximumBlockSize > 0);
        configure (follower, spec.sampleRate);
        envelope.assign (spec.maximumBlockSize, 0.0f);
    }

    void LoudnessTracker::reset()
    {
        follower.reset();
        std::fill (envelope.begin(), envelope.end(), 0.0f);
    }

    void LoudnessTracker::process (const juce::dsp::AudioBlock<float>& input) noexcept
    {
        jassert (input.getNumSamples() <= envelope.size());
        const auto numSamples = juce::jmin (input.getNumSamples(), envelope.size());

        for (size_t i = 0; i < numSamples; ++i)
            envelope[i] = follower.processSample (0, loudestSampleAt (input, i));
    }
}
