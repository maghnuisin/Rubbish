#include "TwistParameterReader.h"
#include "Parameters.h"

#include <stdexcept>

namespace rubbish
{
    namespace
    {
        std::atomic<float>& findRaw (juce::AudioProcessorValueTreeState& state, const juce::String& id)
        {
            auto* value = state.getRawParameterValue (id);

            if (value == nullptr) // a Twist ID is missing from createParameterLayout()
            {
                jassertfalse;
                throw std::logic_error ("Missing parameter: " + id.toStdString());
            }

            return *value;
        }
    }

    TwistParameterReader::TwistParameterReader (juce::AudioProcessorValueTreeState& state)
        : enabled (findRaw (state, ParamIDs::twist)),
          synced (findRaw (state, ParamIDs::twistSync)),
          bells { findBellParameters (state, 0), findBellParameters (state, 1), findBellParameters (state, 2) }
    {
        static_assert (TwistStage::numBells == numTwistBells);
    }

    TwistParameterReader::BellParameters TwistParameterReader::findBellParameters (juce::AudioProcessorValueTreeState& state,
                                                                                 int bellIndex)
    {
        const auto ids = getTwistBellIDs (bellIndex);
        return { &findRaw (state, ids.freq), &findRaw (state, ids.gain), &findRaw (state, ids.q),
                 &findRaw (state, ids.shape), &findRaw (state, ids.speedSync), &findRaw (state, ids.speedFree),
                 &findRaw (state, ids.move), &findRaw (state, ids.start) };
    }

    bool TwistParameterReader::isEnabled() const noexcept
    {
        return enabled.load() >= 0.5f;
    }

    bool TwistParameterReader::isSynced() const noexcept
    {
        return synced.load() >= 0.5f;
    }

    TwistBellSettings TwistParameterReader::readBell (int bellIndex) const noexcept
    {
        const auto& p = bells[static_cast<size_t> (juce::jlimit (0, TwistStage::numBells - 1, bellIndex))];
        const auto shapeIndex = juce::jlimit (0, numTwistShapes - 1, juce::roundToInt (p.shape->load()));

        TwistBellSettings settings;
        settings.frequencyHz = p.frequency->load();
        settings.gainDb = p.gain->load();
        settings.q = p.q->load();
        settings.shape = static_cast<TwistShape> (shapeIndex);
        settings.syncSpeedIndex = juce::roundToInt (p.speedSync->load());
        settings.freeSpeedHz = p.speedFree->load();
        settings.move = p.move->load() * 0.01f;            // percent to -1..+1
        settings.startCycles = p.start->load() / 360.0f;   // degrees to fraction of a cycle
        return settings;
    }
}
