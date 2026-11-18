#pragma once

namespace rubbish
{
    /** The four distortion flavours. Order matches the Character parameter choices. */
    enum class Character
    {
        tape = 0,
        console,
        tube,
        synth
    };

    constexpr int numCharacters = 4;

    /** Pure, stateless transfer curves. Every curve has unity gain for tiny signals
        and never returns a value outside [-1, 1], however hard it is driven. */
    namespace waveshapers
    {
        float tape (float x) noexcept;
        float console (float x) noexcept;
        float tube (float x) noexcept;
        float synth (float x) noexcept;

        float apply (Character character, float x) noexcept;
    }
}
