#pragma once

namespace rubbish
{
    /** The five distortion flavours. Order matches the Character parameter choices. */
    enum class Character
    {
        tape = 0,
        tube,
        heavy,
        edge,
        grit
    };

    constexpr int numCharacters = 5;

    /** Everything one Character curve needs at the current Drive and Push.
        Worked out by CharacterVoicing; used here sample by sample. */
    struct CurveSettings
    {
        float inputGain = 1.0f;       // the Character's own sensitivity (always 1 or more)
        float bias = 0.0f;            // off-centre push that makes the two halves clip differently
        float hardness = 2.0f;        // 2 = soft knee, 8+ = close to a straight clip
        float biasLimitRatio = 1.0e6f; // bias can never exceed this fraction of the signal's own peak
    };

    /** Pure, stateless transfer curves. Every curve has unity gain for tiny signals
        when unbiased and never returns a value outside [-1, 1], however hard it is driven. */
    namespace waveshapers
    {
        /** A clipper whose knee gets harder as hardness rises. */
        float softClip (float x, float hardness) noexcept;

        /** Shapes one sample. signalPeak is the recent peak level of x itself, so the
            bias can be kept smaller than the signal and the sound never collapses. */
        float shape (float x, const CurveSettings& settings, float signalPeak) noexcept;
    }
}
