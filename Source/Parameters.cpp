#include "Parameters.h"

#include <cmath>

namespace rubbish
{
    namespace
    {
        juce::String formatDecibels (float value, int)
        {
            const auto shown = std::abs (value) < 0.05f ? 0.0f : value; // avoid "-0.0 dB"
            return juce::String (shown, 1) + " dB";
        }

        juce::String formatFrequency (float hz, int)
        {
            if (hz >= 1000.0f)
                return juce::String (hz / 1000.0f, 2) + " kHz";

            return juce::String (juce::roundToInt (hz)) + " Hz";
        }

        juce::String formatPercent (float value, int)
        {
            return juce::String (juce::roundToInt (value)) + "%";
        }

        juce::String formatSignedPercent (float value, int)
        {
            const auto rounded = juce::roundToInt (value);
            return (rounded > 0 ? "+" : "") + juce::String (rounded) + "%";
        }

        // Equal knob travel per octave, so the useful low frequencies aren't squashed.
        juce::NormalisableRange<float> makeFrequencyRange (float minHz, float maxHz)
        {
            juce::NormalisableRange<float> range (minHz, maxHz, 0.01f);
            range.setSkewForCentre (std::sqrt (minHz * maxHz));
            return range;
        }

        std::unique_ptr<juce::RangedAudioParameter> makeFrequency (const char* id, const juce::String& name,
                                                                   float minHz, float maxHz, float defaultHz)
        {
            return std::make_unique<juce::AudioParameterFloat> (
                juce::ParameterID { id, parameterVersion },
                name,
                makeFrequencyRange (minHz, maxHz),
                defaultHz,
                juce::AudioParameterFloatAttributes().withLabel ("Hz")
                                                     .withStringFromValueFunction (formatFrequency));
        }

        std::unique_ptr<juce::RangedAudioParameter> makePercent (const char* id, const juce::String& name,
                                                                 float defaultPercent)
        {
            return std::make_unique<juce::AudioParameterFloat> (
                juce::ParameterID { id, parameterVersion },
                name,
                juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f),
                defaultPercent,
                juce::AudioParameterFloatAttributes().withLabel ("%")
                                                     .withStringFromValueFunction (formatPercent));
        }

        std::unique_ptr<juce::RangedAudioParameter> makeFilterOrder()
        {
            return std::make_unique<juce::AudioParameterChoice> (
                juce::ParameterID { ParamIDs::filterOrder, parameterVersion },
                "Filter Order",
                juce::StringArray { "Clean first", "Scream" },
                0);
        }

        std::unique_ptr<juce::RangedAudioParameter> makeOutput()
        {
            return std::make_unique<juce::AudioParameterFloat> (
                juce::ParameterID { ParamIDs::output, parameterVersion },
                "Output",
                juce::NormalisableRange<float> (-24.0f, 12.0f, 0.01f),
                0.0f,
                juce::AudioParameterFloatAttributes().withLabel ("dB")
                                                     .withStringFromValueFunction (formatDecibels));
        }

        std::unique_ptr<juce::RangedAudioParameter> makeFilterFollow()
        {
            return std::make_unique<juce::AudioParameterFloat> (
                juce::ParameterID { ParamIDs::filterFollow, parameterVersion },
                "Filter Follow",
                juce::NormalisableRange<float> (-100.0f, 100.0f, 0.1f),
                0.0f,
                juce::AudioParameterFloatAttributes().withLabel ("%")
                                                     .withStringFromValueFunction (formatSignedPercent));
        }

        std::unique_ptr<juce::RangedAudioParameter> makeToggle (const char* id, const juce::String& name, bool defaultOn)
        {
            return std::make_unique<juce::AudioParameterBool> (juce::ParameterID { id, parameterVersion }, name, defaultOn);
        }

        std::unique_ptr<juce::RangedAudioParameter> makeAutoGain()
        {
            return std::make_unique<juce::AudioParameterBool> (
                juce::ParameterID { ParamIDs::autoGain, parameterVersion },
                "Auto Gain",
                true);
        }

        std::unique_ptr<juce::RangedAudioParameter> makeDrive()
        {
            return std::make_unique<juce::AudioParameterFloat> (
                juce::ParameterID { ParamIDs::drive, parameterVersion },
                "Drive",
                juce::NormalisableRange<float> (0.0f, 36.0f, 0.01f),
                6.0f,
                juce::AudioParameterFloatAttributes().withLabel ("dB")
                                                     .withStringFromValueFunction (formatDecibels));
        }

        std::unique_ptr<juce::RangedAudioParameter> makePush()
        {
            return std::make_unique<juce::AudioParameterBool> (
                juce::ParameterID { ParamIDs::push, parameterVersion },
                "Push",
                false);
        }

        std::unique_ptr<juce::RangedAudioParameter> makeCharacter()
        {
            return std::make_unique<juce::AudioParameterChoice> (
                juce::ParameterID { ParamIDs::character, parameterVersion },
                "Character",
                juce::StringArray { "Tape", "Console", "Tube", "Synth" },
                0);
        }
    }

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        layout.add (makeDrive(), makePush(), makeCharacter(),
                    makeFrequency (ParamIDs::cleanLow, "Clean Low", 20.0f, 500.0f, 20.0f));

        layout.add (makeFrequency (ParamIDs::lowCut, "Low Cut", 20.0f, 2000.0f, 20.0f),
                    makePercent (ParamIDs::lowPeak, "Low Peak", 0.0f),
                    makeFrequency (ParamIDs::highCut, "High Cut", 200.0f, 20000.0f, 20000.0f),
                    makePercent (ParamIDs::highPeak, "High Peak", 0.0f),
                    makeFilterOrder(),
                    makeFilterFollow());

        layout.add (makePercent (ParamIDs::dynamicsRestore, "Dynamics Restore", 0.0f),
                    makePercent (ParamIDs::spread, "Spread", 0.0f),
                    makeAutoGain(),
                    makePercent (ParamIDs::mix, "Mix", 100.0f),
                    makeOutput(),
                    makeToggle (ParamIDs::delta, "Delta", false));
        return layout;
    }
}
