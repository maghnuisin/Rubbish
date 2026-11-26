#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include <stdexcept>

namespace rubbish
{
    /** The live value of a parameter, for lock-free reading on the audio thread.
        Fails loudly if the ID is missing from createParameterLayout(). */
    inline std::atomic<float>& findRawParameter (juce::AudioProcessorValueTreeState& state, const juce::String& id)
    {
        auto* value = state.getRawParameterValue (id);

        if (value == nullptr)
        {
            jassertfalse;
            throw std::logic_error ("Missing parameter: " + id.toStdString());
        }

        return *value;
    }
}
