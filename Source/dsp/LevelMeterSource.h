#pragma once

#include <juce_dsp/juce_dsp.h>

#include <atomic>

namespace rubbish
{
    /** The one read-only path from audio to window: the loudest output sample
        since the window last looked. Lock-free, so it is safe in processBlock(). */
    class LevelMeterSource
    {
    public:
        void measure (const juce::dsp::AudioBlock<float>& block) noexcept
        {
            storeIfLouder (findPeak (block));
        }

        /** Called by the window: returns the peak and starts collecting a new one. */
        float takePeak() noexcept { return peak.exchange (0.0f, std::memory_order_relaxed); }

    private:
        static float findPeak (const juce::dsp::AudioBlock<float>& block) noexcept
        {
            float loudest = 0.0f;

            for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
            {
                const auto* data = block.getChannelPointer (ch);

                for (size_t i = 0; i < block.getNumSamples(); ++i)
                    loudest = juce::jmax (loudest, std::abs (data[i]));
            }

            return loudest;
        }

        void storeIfLouder (float candidate) noexcept
        {
            auto stored = peak.load (std::memory_order_relaxed);

            while (candidate > stored
                   && ! peak.compare_exchange_weak (stored, candidate, std::memory_order_relaxed))
            {
            }
        }

        std::atomic<float> peak { 0.0f };
    };
}
