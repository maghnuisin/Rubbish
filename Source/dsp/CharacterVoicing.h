#pragma once

#include "Waveshapers.h"

namespace rubbish
{
    /** Where Drive and Push currently sit. */
    struct DriveState
    {
        float driveDb = 0.0f;  // the Drive knob, 0..36 dB
        float totalDb = 0.0f;  // Drive plus Push's extra gain
        float push = 0.0f;     // 0 = off, 1 = on (fades between when switched)
    };

    /** Turns a Character plus the current Drive and Push into curve settings.
        The numbers behind each Character were fitted to measurements of Soundtoys
        Decapitator's styles (see decapitator-analysis.md):
        Tape = A, Tube = T, Heavy = E, Edge = N, Grit = P. */
    CurveSettings computeCurveSettings (Character character, const DriveState& drive) noexcept;
}
