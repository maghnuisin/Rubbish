#include "StateMigration.h"
#include "Parameters.h"

namespace rubbish
{
    namespace
    {
        const juce::Identifier paramType { "PARAM" };
        const juce::Identifier idProperty { "id" };
        const juce::Identifier valueProperty { "value" };

        juce::ValueTree findParam (const juce::ValueTree& state, const juce::String& id)
        {
            return state.getChildWithProperty (idProperty, id);
        }

        void setParam (juce::ValueTree& state, const juce::String& id, const juce::var& value)
        {
            auto param = findParam (state, id);

            if (! param.isValid())
            {
                param = juce::ValueTree (paramType);
                param.setProperty (idProperty, id, nullptr);
                state.appendChild (param, nullptr);
            }

            param.setProperty (valueProperty, value, nullptr);
        }

        // Copies an old setting to its new home (if it was saved) and removes the old one.
        void moveParam (juce::ValueTree& state, const juce::String& oldId, const juce::String& newId)
        {
            const auto oldParam = findParam (state, oldId);

            if (! oldParam.isValid())
                return;

            setParam (state, newId, oldParam.getProperty (valueProperty));
            state.removeChild (oldParam, nullptr);
        }

        bool isVersion2State (const juce::ValueTree& state)
        {
            return findParam (state, "bell1Move").isValid() && ! findParam (state, getLfoIDs (0).shape).isValid();
        }

        void copySharedSync (juce::ValueTree& state)
        {
            const auto oldSync = findParam (state, "twistSync");

            if (! oldSync.isValid())
                return;

            for (int lfo = 0; lfo < numLfos; ++lfo)
                setParam (state, getLfoIDs (lfo).sync, oldSync.getProperty (valueProperty));

            state.removeChild (oldSync, nullptr);
        }
    }

    void upgradeVersion2State (juce::ValueTree& state)
    {
        if (! isVersion2State (state))
            return;

        for (int bell = 0; bell < numTwistBells; ++bell)
        {
            const auto oldPrefix = "bell" + juce::String (bell + 1);
            const auto lfo = getLfoIDs (bell);

            moveParam (state, oldPrefix + "Shape", lfo.shape);
            moveParam (state, oldPrefix + "SpeedSync", lfo.speedSync);
            moveParam (state, oldPrefix + "SpeedFree", lfo.speedFree);
            moveParam (state, oldPrefix + "Start", lfo.start);
            moveParam (state, oldPrefix + "Move", getModAmountID (bell, bellFreqTarget (bell)));
        }

        copySharedSync (state);
    }
}
