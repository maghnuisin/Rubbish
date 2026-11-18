#pragma once

#include <array>
#include <atomic>

namespace rubbish
{
    /** Read-only path from audio to window for the Twist display: where each bell
        really is right now, after its LFO has moved it. Lock-free, so it is safe in
        processBlock(). Owner-approved exception to "talk only through parameters",
        like LevelMeterSource; it carries bell positions and nothing else. */
    class TwistDisplaySource
    {
    public:
        static constexpr int numBells = 3;

        void publishFrequency (int bellIndex, float frequencyHz) noexcept
        {
            if (bellIndex >= 0 && bellIndex < numBells)
                frequencies[static_cast<size_t> (bellIndex)].store (frequencyHz, std::memory_order_relaxed);
        }

        /** Returns 0 until the audio side has published a position. */
        float getFrequency (int bellIndex) const noexcept
        {
            if (bellIndex < 0 || bellIndex >= numBells)
                return 0.0f;

            return frequencies[static_cast<size_t> (bellIndex)].load (std::memory_order_relaxed);
        }

    private:
        std::array<std::atomic<float>, numBells> frequencies {};
    };
}
