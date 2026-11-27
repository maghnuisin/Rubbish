// Opens Rubbish's editor off-screen and saves it as a PNG at 2x resolution.
// Usage: RubbishScreenshot <output.png>
#include "PluginProcessor.h"

#include <juce_audio_utils/juce_audio_utils.h>

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();

// A typical "in use" setting, so the screenshot shows the controls doing something.
static void applyShowcaseSettings (juce::AudioProcessor& processor)
{
    const std::map<juce::String, float> settings {
        { "twist", 1.0f },  { "bell1Gain", 9.0f }, { "bell2Gain", -6.0f }, { "bell3Gain", 7.0f },
        { "bell1Q", 60.0f }, { "drive", 20.0f },   { "character", 4.0f },  { "lowPeak", 35.0f },
        { "highCut", 8000.0f }, { "highPeak", 40.0f }, { "filterFollow", 30.0f },
        { "dynamicsRestore", 30.0f }, { "spread", 25.0f }
    };

    for (auto* parameter : processor.getParameters())
        if (auto* withId = dynamic_cast<juce::RangedAudioParameter*> (parameter))
            if (auto it = settings.find (withId->getParameterID()); it != settings.end())
                withId->setValueNotifyingHost (withId->convertTo0to1 (it->second));
}

int main (int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "usage: RubbishScreenshot <output.png>\n";
        return 1;
    }

    juce::ScopedJuceInitialiser_GUI gui;
    std::unique_ptr<juce::AudioProcessor> processor (createPluginFilter());
    processor->setPlayConfigDetails (2, 2, 48000.0, 512);
    processor->prepareToPlay (48000.0, 512);
    applyShowcaseSettings (*processor);

    std::unique_ptr<juce::AudioProcessorEditor> editor (processor->createEditorAndMakeActive());
    // Let timers (meters, Twist display) tick once so everything is drawn in its live state.
    juce::MessageManager::getInstance()->runDispatchLoopUntil (300);

    const auto image = editor->createComponentSnapshot (editor->getLocalBounds(), true, 2.0f);
    juce::File output (juce::File::getCurrentWorkingDirectory().getChildFile (argv[1]));
    output.deleteFile();
    juce::FileOutputStream stream (output);
    if (! stream.openedOk() || ! juce::PNGImageFormat().writeImageToStream (image, stream))
    {
        std::cerr << "could not write " << output.getFullPathName() << "\n";
        return 1;
    }

    editor.reset();
    processor->releaseResources();
    std::cout << "saved " << output.getFullPathName() << " (" << image.getWidth() << "x" << image.getHeight() << ")\n";
    return 0;
}
