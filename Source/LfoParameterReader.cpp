#include "LfoParameterReader.h"
#include "Parameters.h"
#include "RawParameterLookup.h"

namespace rubbish
{
    namespace
    {
        constexpr float percentToFraction = 0.01f;
    }

    LfoParameterReader::LfoParameterReader (juce::AudioProcessorValueTreeState& state)
        : lfos { findLfoParameters (state, 0), findLfoParameters (state, 1), findLfoParameters (state, 2) },
          amounts { findAmounts (state, 0), findAmounts (state, 1), findAmounts (state, 2) }
    {
        static_assert (LfoBank::numLfos == numLfos);
        static_assert (ModulationMatrix::numTargets == numModTargets);
    }

    LfoParameterReader::LfoParameters LfoParameterReader::findLfoParameters (juce::AudioProcessorValueTreeState& state,
                                                                             int lfoIndex)
    {
        const auto ids = getLfoIDs (lfoIndex);
        auto find = [&state] (const juce::String& id) { return &findRawParameter (state, id); };
        return { find (ids.shape), find (ids.curve), find (ids.sync), find (ids.speedSync), find (ids.speedFree),
                 find (ids.start), find (ids.smooth), find (ids.jitter), find (ids.offset), find (ids.unipolar) };
    }

    LfoParameterReader::AmountRow LfoParameterReader::findAmounts (juce::AudioProcessorValueTreeState& state, int lfoIndex)
    {
        AmountRow row {};

        for (int target = 0; target < numModTargets; ++target)
            row[static_cast<size_t> (target)] = &findRawParameter (state, getModAmountID (lfoIndex, target));

        return row;
    }

    LfoSettings LfoParameterReader::readLfo (int lfoIndex) const noexcept
    {
        const auto& p = lfos[static_cast<size_t> (juce::jlimit (0, LfoBank::numLfos - 1, lfoIndex))];
        const auto shapeIndex = juce::jlimit (0, numTwistShapes - 1, juce::roundToInt (p.shape->load()));

        LfoSettings settings;
        settings.shape.shape = static_cast<TwistShape> (shapeIndex);
        settings.shape.curve = p.curve->load() * percentToFraction;
        settings.shape.smooth = p.smooth->load() * percentToFraction;
        settings.shape.jitter = p.jitter->load() * percentToFraction;
        settings.shape.offset = p.offset->load() * percentToFraction;
        settings.shape.unipolar = p.unipolar->load() >= 0.5f;
        settings.synced = p.sync->load() >= 0.5f;
        settings.syncSpeedIndex = juce::roundToInt (p.speedSync->load());
        settings.freeSpeedHz = p.speedFree->load();
        settings.startCycles = p.start->load() / 360.0f; // degrees to fraction of a cycle
        return settings;
    }

    float LfoParameterReader::readAmount (int lfoIndex, int targetIndex) const noexcept
    {
        const auto lfo = static_cast<size_t> (juce::jlimit (0, LfoBank::numLfos - 1, lfoIndex));
        const auto target = static_cast<size_t> (juce::jlimit (0, ModulationMatrix::numTargets - 1, targetIndex));
        return amounts[lfo][target]->load() * percentToFraction;
    }
}
