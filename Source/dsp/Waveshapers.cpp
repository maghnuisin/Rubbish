#include "Waveshapers.h"

#include <algorithm>
#include <cmath>

namespace rubbish::waveshapers
{
    float softClip (float x, float hardness) noexcept
    {
        return x / std::pow (1.0f + std::pow (std::abs (x), hardness), 1.0f / hardness);
    }

    // Biasing the curve off-centre makes the two halves of the wave clip differently.
    // That asymmetry creates even harmonics (weight, thickness). Capping the bias below
    // the signal's own peak keeps the wave crossing the curve's centre, so even a quiet
    // signal at full Drive stays audible. The DC offset this introduces is removed later.
    float shape (float x, const CurveSettings& settings, float signalPeak) noexcept
    {
        const auto driven = settings.inputGain * x;
        const auto biasLimit = settings.biasLimitRatio * settings.inputGain * signalPeak;
        const auto bias = std::clamp (settings.bias, -biasLimit, biasLimit);
        const auto centre = softClip (bias, settings.hardness);

        // Dividing by inputGain keeps quiet signals at their original level;
        // dividing by (1 + |centre|) keeps the output inside [-1, 1].
        return (softClip (driven + bias, settings.hardness) - centre)
               / (settings.inputGain * (1.0f + std::abs (centre)));
    }
}
