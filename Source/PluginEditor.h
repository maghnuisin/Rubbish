#pragma once

#include "PluginProcessor.h"
#include "ui/MainLayout.h"

class RubbishAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit RubbishAudioProcessorEditor (RubbishAudioProcessor&);
    ~RubbishAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void configureResizing();
    static void configureColours (juce::LookAndFeel_V4& lookAndFeel);

    // Declared first so it outlives every component that uses it.
    juce::LookAndFeel_V4 lookAndFeel;
    juce::SharedResourcePointer<juce::TooltipWindow> tooltipWindow;

    rubbish::MainLayout layout;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RubbishAudioProcessorEditor)
};
