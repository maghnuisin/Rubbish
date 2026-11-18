#include "PluginEditor.h"

namespace
{
    // The window can shrink to 90% of the design size (text stays readable on a
    // laptop) and grow to 180%.
    constexpr double minimumScale = 0.9;
    constexpr double maximumScale = 1.8;
}

RubbishAudioProcessorEditor::RubbishAudioProcessorEditor (RubbishAudioProcessor& owner)
    : AudioProcessorEditor (owner),
      layout (owner.getValueTreeState(), owner.getInputLevel(), owner.getOutputLevel(),
              owner.getTwistDisplaySource())
{
    configureColours (lookAndFeel);
    setLookAndFeel (&lookAndFeel);

    addAndMakeVisible (layout);
    configureResizing();
}

RubbishAudioProcessorEditor::~RubbishAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void RubbishAudioProcessorEditor::configureResizing()
{
    using rubbish::MainLayout;
    const auto scaled = [] (int size, double scale) { return juce::roundToInt (size * scale); };

    setResizable (true, true);
    setResizeLimits (scaled (MainLayout::designWidth, minimumScale), scaled (MainLayout::designHeight, minimumScale),
                     scaled (MainLayout::designWidth, maximumScale), scaled (MainLayout::designHeight, maximumScale));
    getConstrainer()->setFixedAspectRatio (static_cast<double> (MainLayout::designWidth) / MainLayout::designHeight);
    setSize (MainLayout::designWidth, MainLayout::designHeight);
}

// Lit buttons (Push, the selected Character, Auto Gain, Delta) use the same blue as
// the knob dots, so it's obvious at a glance what is switched on.
void RubbishAudioProcessorEditor::configureColours (juce::LookAndFeel_V4& lookAndFeel)
{
    const auto accent = lookAndFeel.findColour (juce::Slider::thumbColourId);
    lookAndFeel.setColour (juce::TextButton::buttonOnColourId, accent);
    lookAndFeel.setColour (juce::TextButton::textColourOnId, juce::Colours::black);
}

void RubbishAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

// The layout is always arranged at its design size, then scaled as a whole.
void RubbishAudioProcessorEditor::resized()
{
    using rubbish::MainLayout;
    const auto scale = static_cast<float> (getWidth()) / static_cast<float> (MainLayout::designWidth);

    layout.setBounds (0, 0, MainLayout::designWidth, MainLayout::designHeight);
    layout.setTransform (juce::AffineTransform::scale (scale));
}
