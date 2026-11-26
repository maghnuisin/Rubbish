#include "CharacterVoicing.h"

#include <algorithm>
#include <cmath>

namespace rubbish
{
    namespace
    {
        /** How one Character changes as Drive and Push move. */
        struct Voicing
        {
            float sensitivityDb;  // extra internal gain: higher = starts distorting sooner
            float biasAtZero;     // bias with Drive at 0
            float biasAcrossDrive;// bias added by the time Drive reaches 36 dB
            float biasFromPush;   // bias added by Push
            float hardness;       // knee hardness with Push off
            float hardnessFromPush;
            float biasFollowsGain;// 0 = fixed bias; towards 1 = bias grows with the total gain, like a tube's bias shift
            float biasLimitRatio; // bias never exceeds this fraction of the signal peak
        };

        constexpr float maxDriveDb = 36.0f;
        constexpr float unlimited = 1.0e6f;

        // Fitted offline to the Decapitator THD and even/odd measurements.
        constexpr Voicing tapeVoicing  {  4.738f, 0.666f, -0.574f, -1.003f, 5.687f, 0.000f, 0.000f, unlimited };
        constexpr Voicing tubeVoicing  {  5.444f, 0.050f, -0.122f,  0.119f, 1.752f, 1.957f, 0.871f, 0.413f };
        constexpr Voicing heavyVoicing {  5.582f, 0.200f, -0.925f, -0.066f, 1.785f, 0.000f, 0.918f, 0.662f };
        constexpr Voicing edgeVoicing  {  8.008f, 0.552f, -1.135f, -0.448f, 7.999f, 0.000f, 0.000f, unlimited };
        // Grit was refitted (2026-11-26) to Drive sweeps of P recorded in Logic at a real
        // bass level (about -13 dBFS peak), matching the harmonic mix, not just the total.
        constexpr Voicing gritVoicing  {  2.198f, 0.319f, -0.824f, -0.417f, 2.166f, -1.021f, 0.695f, 0.950f };

        const Voicing& voicingFor (Character character) noexcept
        {
            switch (character)
            {
                case Character::tape:  return tapeVoicing;
                case Character::tube:  return tubeVoicing;
                case Character::heavy: return heavyVoicing;
                case Character::edge:  return edgeVoicing;
                case Character::grit:  return gritVoicing;
            }

            return tapeVoicing;
        }

        float decibelsToGain (float db) noexcept
        {
            return std::pow (10.0f, db / 20.0f);
        }

        float biasFor (const Voicing& v, const DriveState& drive) noexcept
        {
            const auto driveFraction = std::clamp (drive.driveDb / maxDriveDb, 0.0f, 1.0f);
            const auto baseBias = v.biasAtZero + v.biasAcrossDrive * driveFraction + v.biasFromPush * drive.push;
            const auto totalGain = decibelsToGain (drive.totalDb + v.sensitivityDb);
            return baseBias * std::pow (totalGain, v.biasFollowsGain);
        }
    }

    CurveSettings computeCurveSettings (Character character, const DriveState& drive) noexcept
    {
        const auto& v = voicingFor (character);

        CurveSettings settings;
        settings.inputGain = decibelsToGain (v.sensitivityDb);
        settings.bias = biasFor (v, drive);
        settings.hardness = v.hardness + v.hardnessFromPush * drive.push;
        settings.biasLimitRatio = v.biasLimitRatio;
        return settings;
    }
}
