#include "Parameters.h"
#include "dsp/TwistClock.h"

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

        // ---- Twist (version 2) ----

        struct TwistBellDefaults
        {
            float freqHz, gainDb, qPercent;
            int syncSpeedIndex;
            float freeSpeedHz, movePercent, startDegrees;
        };

        // Bell 1 / 2 / 3 defaults from prd.md: flat (0 dB) and still (0% Move) until you
        // dial them in, with different speeds and start points ready for when they move.
        constexpr TwistBellDefaults twistDefaults[numTwistBells] {
            { 200.0f,  0.0f, 37.5f, 12, 0.5f, 0.0f,   0.0f }, // 1 bar
            { 700.0f,  0.0f, 37.5f, 13, 0.3f, 0.0f, 120.0f }, // 2 bars
            { 2000.0f, 0.0f, 37.5f, 10, 0.8f, 0.0f, 240.0f }  // 1/2
        };

        juce::ParameterID twistID (const juce::String& id)
        {
            return juce::ParameterID { id, twistParameterVersion };
        }

        juce::String formatHertz (float value, int)
        {
            return juce::String (value, value < 1.0f ? 2 : 1) + " Hz";
        }

        juce::String formatDegrees (float value, int)
        {
            return juce::String (juce::roundToInt (value)) + juce::String::charToString (static_cast<juce::juce_wchar> (0x00b0));
        }

        // Log-style travel, so small values get as much knob as large ones.
        juce::NormalisableRange<float> makeLogRange (float minimum, float maximum, float interval)
        {
            juce::NormalisableRange<float> range (minimum, maximum, interval);
            range.setSkewForCentre (std::sqrt (minimum * maximum));
            return range;
        }

        std::unique_ptr<juce::RangedAudioParameter> makeTwistFloat (const juce::String& id, const juce::String& name,
                                                                    juce::NormalisableRange<float> range, float defaultValue,
                                                                    const juce::String& unit,
                                                                    juce::String (*format) (float, int))
        {
            return std::make_unique<juce::AudioParameterFloat> (
                twistID (id), name, range, defaultValue,
                juce::AudioParameterFloatAttributes().withLabel (unit).withStringFromValueFunction (format));
        }

        std::unique_ptr<juce::AudioProcessorParameterGroup> makeTwistBellGroup (int bellIndex)
        {
            const auto ids = getTwistBellIDs (bellIndex);
            const auto& d = twistDefaults[bellIndex];
            const auto prefix = "Bell " + juce::String (bellIndex + 1) + " ";

            return std::make_unique<juce::AudioProcessorParameterGroup> (
                "bell" + juce::String (bellIndex + 1), prefix.trim(), "|",
                makeTwistFloat (ids.freq, prefix + "Freq", makeFrequencyRange (20.0f, 20000.0f), d.freqHz, "Hz", formatFrequency),
                makeTwistFloat (ids.gain, prefix + "Gain", { -24.0f, 24.0f, 0.01f }, d.gainDb, "dB", formatDecibels),
                makeTwistFloat (ids.q, prefix + "Q", { 0.0f, 100.0f, 0.1f }, d.qPercent, "%", formatPercent),
                std::make_unique<juce::AudioParameterChoice> (twistID (ids.shape), prefix + "Shape", twistShapeNames, 0),
                std::make_unique<juce::AudioParameterChoice> (twistID (ids.speedSync), prefix + "Speed (Sync)",
                                                              twistSyncSpeedNames, d.syncSpeedIndex),
                makeTwistFloat (ids.speedFree, prefix + "Speed (Free)", makeLogRange (0.01f, 10.0f, 0.001f),
                                d.freeSpeedHz, "Hz", formatHertz),
                makeTwistFloat (ids.move, prefix + "Move", { -100.0f, 100.0f, 0.1f }, d.movePercent, "%", formatSignedPercent),
                makeTwistFloat (ids.start, prefix + "Start", { 0.0f, 360.0f, 1.0f }, d.startDegrees, "deg", formatDegrees));
        }

        void addTwistParameters (juce::AudioProcessorValueTreeState::ParameterLayout& layout)
        {
            layout.add (std::make_unique<juce::AudioParameterBool> (twistID (ParamIDs::twist), "Twist", false),
                        std::make_unique<juce::AudioParameterBool> (twistID (ParamIDs::twistSync), "Twist Sync", true));

            for (int bell = 0; bell < numTwistBells; ++bell)
                layout.add (makeTwistBellGroup (bell));
        }
    }

    TwistBellIDs getTwistBellIDs (int bellIndex)
    {
        jassert (bellIndex >= 0 && bellIndex < numTwistBells);
        const auto prefix = "bell" + juce::String (bellIndex + 1);
        return { prefix + "Freq", prefix + "Gain", prefix + "Q", prefix + "Shape",
                 prefix + "SpeedSync", prefix + "SpeedFree", prefix + "Move", prefix + "Start" };
    }

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        jassert (twistSyncSpeedNames.size() == TwistClock::numSyncSpeeds);
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

        addTwistParameters (layout);
        return layout;
    }
}
