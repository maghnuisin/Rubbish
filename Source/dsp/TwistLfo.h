#pragma once

#include <juce_core/juce_core.h>

namespace rubbish
{
    /** Twist LFO shapes. Matches the order of twistShapeNames in Parameters.h. */
    enum class TwistShape
    {
        smooth = 0,
        triangle,
        ramp,
        randomWander
    };

    inline constexpr int numTwistShapes = 4;

    /** One Twist bell's LFO. The caller says where in its cycle it is (so tempo
        sync and free running share one LFO), and it returns a value from -1 to +1.

        The output is lightly smoothed, so the Ramp's snap back, shape changes and
        Sync jumps glide over a few milliseconds instead of clicking. */
    class TwistLfo
    {
    public:
        explicit TwistLfo (int randomSeed) noexcept : seed (randomSeed) {}

        void prepare (double sampleRate);
        void reset() noexcept;

        void setShape (TwistShape newShape) noexcept { shape = newShape; }

        /** cyclePosition counts whole cycles; only its fractional part (and, for
            Random wander, which cycle it is) matters. */
        float next (double cyclePosition) noexcept;

    private:
        float shapeAt (double cyclePosition) const noexcept;
        float randomWanderAt (double cyclePosition) const noexcept;
        float randomPointFor (juce::int64 cycleIndex) const noexcept;

        static constexpr double smoothingSeconds = 0.005;

        const int seed;
        TwistShape shape = TwistShape::smooth;
        float smoothingCoefficient = 0.0f;
        float smoothedValue = 0.0f;
        bool hasStarted = false;
    };
}
