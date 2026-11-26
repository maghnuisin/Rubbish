#pragma once

#include <juce_data_structures/juce_data_structures.h>

namespace rubbish
{
    /** Opens projects saved with version 2 so they sound the same in version 3:
        each bell's Move becomes the ring amount from its LFO onto its Freq, and its
        Shape, Speed and Start move into the matching LFO (the shared Sync is copied
        to all three). Does nothing to version 1 or version 3 state. */
    void upgradeVersion2State (juce::ValueTree& state);
}
