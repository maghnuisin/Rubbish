#pragma once

#include <juce_data_structures/juce_data_structures.h>

namespace rubbish
{
    /** Opens projects saved with version 2 so they sound the same in version 3:
        each bell's Move becomes the ring amount from its LFO onto its Freq, and its
        Shape, Speed and Start move into the matching LFO (the shared Sync is copied
        to all three). Does nothing to version 1 or version 3 state. */
    void upgradeVersion2State (juce::ValueTree& state);

    /** Projects saved before the Character redesign used Tape / Console / Tube / Synth.
        This moves them onto the new list (Tape / Tube / Heavy / Edge / Grit):
        Console becomes Edge (clean, then bites) and Synth becomes Heavy (most aggressive).
        Does nothing to projects saved with the new list. */
    void upgradeCharacterChoices (juce::ValueTree& state);

    /** Tags the plugin's state as using the new Character list, so it is saved with it. */
    void markCurrentCharacterChoices (juce::ValueTree& state);
}
