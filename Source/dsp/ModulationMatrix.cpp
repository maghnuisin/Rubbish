#include "ModulationMatrix.h"
#include "LfoBank.h"

#include <algorithm>

namespace rubbish
{
    static_assert (ModulationMatrix::numLfos == LfoBank::numLfos);

    void ModulationMatrix::prepare (double sampleRate, int maximumBlockSize)
    {
        jassert (sampleRate > 0.0 && maximumBlockSize > 0);

        for (auto& lfoAmounts : amounts)
            for (auto& amount : lfoAmounts)
                amount.reset (sampleRate, smoothingSeconds);

        for (auto& buffer : movement)
            buffer.assign (static_cast<size_t> (maximumBlockSize), 0.0f);

        reset();
    }

    void ModulationMatrix::reset() noexcept
    {
        for (auto& lfoAmounts : amounts)
            for (auto& amount : lfoAmounts)
                amount.setCurrentAndTargetValue (amount.getTargetValue());
    }

    void ModulationMatrix::setAmount (int lfoIndex, int targetIndex, float amount) noexcept
    {
        jassert (lfoIndex >= 0 && lfoIndex < numLfos && targetIndex >= 0 && targetIndex < numTargets);
        amounts[static_cast<size_t> (juce::jlimit (0, numLfos - 1, lfoIndex))]
               [static_cast<size_t> (juce::jlimit (0, numTargets - 1, targetIndex))]
            .setTargetValue (juce::jlimit (-1.0f, 1.0f, amount));
    }

    // A ring at 0% that has finished gliding there costs nothing.
    bool ModulationMatrix::isConnected (int targetIndex) const noexcept
    {
        for (const auto& lfoAmounts : amounts)
        {
            const auto& amount = lfoAmounts[static_cast<size_t> (targetIndex)];

            if (amount.isSmoothing() || amount.getTargetValue() != 0.0f)
                return true;
        }

        return false;
    }

    void ModulationMatrix::mixTarget (int targetIndex, const LfoBank& lfos, int numSamples) noexcept
    {
        auto* out = movement[static_cast<size_t> (targetIndex)].data();
        std::fill (out, out + numSamples, 0.0f);

        for (int lfo = 0; lfo < numLfos; ++lfo)
        {
            auto& amount = amounts[static_cast<size_t> (lfo)][static_cast<size_t> (targetIndex)];
            const auto* values = lfos.getValues (lfo);

            for (int i = 0; i < numSamples; ++i)
                out[i] += amount.getNextValue() * values[i];
        }
    }

    void ModulationMatrix::process (const LfoBank& lfos, int numSamples) noexcept
    {
        jassert (numSamples <= static_cast<int> (movement[0].size()));
        numSamples = juce::jmin (numSamples, static_cast<int> (movement[0].size()));

        for (int target = 0; target < numTargets; ++target)
        {
            moving[static_cast<size_t> (target)] = isConnected (target);

            if (moving[static_cast<size_t> (target)])
                mixTarget (target, lfos, numSamples);
        }
    }

    const float* ModulationMatrix::getModulation (int targetIndex) const noexcept
    {
        if (targetIndex < 0 || targetIndex >= numTargets || ! moving[static_cast<size_t> (targetIndex)])
            return nullptr;

        return movement[static_cast<size_t> (targetIndex)].data();
    }
}
