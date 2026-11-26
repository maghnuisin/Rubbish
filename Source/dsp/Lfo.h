#pragma once

#include <juce_core/juce_core.h>

namespace rubbish
{
    /** LFO shapes. Matches the order of twistShapeNames in Parameters.h. */
    enum class TwistShape
    {
        smooth = 0,
        triangle,
        ramp,
        randomWander
    };

    inline constexpr int numTwistShapes = 4;

    /** The settings that change how one LFO's wave looks (speed and timing live in LfoBank). */
    struct LfoShapeSettings
    {
        TwistShape shape = TwistShape::smooth;
        float curve = 0.0f;   // -1 to +1; positive rises fast and falls slowly
        float smooth = 0.0f;  // 0 to 1
        float jitter = 0.0f;  // 0 to 1
        float offset = 0.0f;  // -1 to +1
        bool unipolar = false;
    };

    /** One LFO. The caller says where in its cycle it is (so tempo sync and free
        running share one LFO), and it returns -1..+1 (or 0..+1 when Unipolar).

        Everything random comes from the cycle position, so with Sync on the same
        bar always moves the same way. The output is always lightly smoothed, so
        the Ramp's snap back, shape changes and Sync jumps never click. */
    class Lfo
    {
    public:
        explicit Lfo (int randomSeed) noexcept : seed (randomSeed) {}

        void prepare (double sampleRate);
        void reset() noexcept;

        /** cycleSeconds is how long one cycle currently lasts; Smooth scales with it. */
        void setShape (const LfoShapeSettings& newSettings, double cycleSeconds) noexcept;

        /** cyclePosition counts whole cycles; only its fractional part (and, for
            the random parts, which cycle it is) matters. */
        float next (double cyclePosition) noexcept;

    private:
        float shapeAt (double cyclePosition) const noexcept;
        float curvedWaveAt (double phase) const noexcept;
        float randomWanderAt (double cyclePosition) const noexcept;
        float jitterAt (double cyclePosition) const noexcept;
        float randomPointFor (juce::int64 index, int stream) const noexcept;
        float applyOffsetAndPolarity (float value) const noexcept;

        static constexpr double minimumSmoothingSeconds = 0.005;
        static constexpr double maximumSmoothingCycles = 0.25; // Smooth at 100% glides over a quarter cycle
        static constexpr int jitterPointsPerCycle = 16;

        const int seed;
        double sampleRate = 44100.0;
        LfoShapeSettings settings;
        float smoothingCoefficient = 0.0f;
        float smoothedValue = 0.0f;
        bool hasStarted = false;
    };
}
