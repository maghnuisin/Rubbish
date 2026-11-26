#include "TwistParameterReader.h"
#include "Parameters.h"
#include "RawParameterLookup.h"

namespace rubbish
{
    TwistParameterReader::TwistParameterReader (juce::AudioProcessorValueTreeState& state)
        : enabled (findRawParameter (state, ParamIDs::twist)),
          bells { findBellParameters (state, 0), findBellParameters (state, 1), findBellParameters (state, 2) }
    {
        static_assert (TwistStage::numBells == numTwistBells);
    }

    TwistParameterReader::BellParameters TwistParameterReader::findBellParameters (juce::AudioProcessorValueTreeState& state,
                                                                                 int bellIndex)
    {
        const auto ids = getTwistBellIDs (bellIndex);
        return { &findRawParameter (state, ids.freq), &findRawParameter (state, ids.gain), &findRawParameter (state, ids.q) };
    }

    bool TwistParameterReader::isEnabled() const noexcept
    {
        return enabled.load() >= 0.5f;
    }

    TwistBellSettings TwistParameterReader::readBell (int bellIndex) const noexcept
    {
        const auto& p = bells[static_cast<size_t> (juce::jlimit (0, TwistStage::numBells - 1, bellIndex))];
        return { p.frequency->load(), p.gain->load(), p.q->load() };
    }
}
