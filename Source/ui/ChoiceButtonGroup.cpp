#include "ChoiceButtonGroup.h"

namespace rubbish
{
    ChoiceButtonGroup::ChoiceButtonGroup (juce::RangedAudioParameter& choiceParameter, int numColumns)
        : columns (numColumns),
          attachment (choiceParameter, [this] (float value) { selectButton (juce::roundToInt (value)); })
    {
        jassert (numColumns > 0);
        createButtons (choiceParameter.getAllValueStrings());
        attachment.sendInitialUpdate();
    }

    void ChoiceButtonGroup::createButtons (const juce::StringArray& names)
    {
        jassert (! names.isEmpty()); // only choice parameters list their values

        const auto radioGroupId = juce::Random::getSystemRandom().nextInt (1 << 30) + 1;

        for (int i = 0; i < names.size(); ++i)
        {
            auto* button = buttons.add (std::make_unique<juce::TextButton> (names[i]));
            button->setClickingTogglesState (true);
            button->setRadioGroupId (radioGroupId);
            button->onClick = [this, i]
            {
                if (buttons[i]->getToggleState())
                    attachment.setValueAsCompleteGesture (static_cast<float> (i));
            };
            addAndMakeVisible (button);
        }
    }

    void ChoiceButtonGroup::selectButton (int index)
    {
        if (auto* button = buttons[index])
            button->setToggleState (true, juce::dontSendNotification);
    }

    void ChoiceButtonGroup::resized()
    {
        const auto rows = (buttons.size() + columns - 1) / columns;
        const auto cellWidth = getWidth() / columns;
        const auto cellHeight = getHeight() / juce::jmax (1, rows);

        for (int i = 0; i < buttons.size(); ++i)
        {
            const juce::Rectangle<int> cell ((i % columns) * cellWidth, (i / columns) * cellHeight,
                                             cellWidth, cellHeight);
            buttons[i]->setBounds (cell.reduced (buttonGap));
        }
    }
}
