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
            float biasLimitRatio; // bias never exceeds this fraction of the signal peak (Drive at 0)
            float biasLimitAcrossDrive = 0.0f; // added to biasLimitRatio by the time Drive reaches 36 dB
            float hardnessAcrossDrive = 0.0f;  // added to hardness by the time Drive reaches 36 dB
        };

        constexpr float maxDriveDb = 36.0f;
        constexpr float unlimited = 1.0e6f;

        // Fitted offline to the Decapitator THD and even/odd measurements.
        constexpr Voicing tapeVoicing  {  4.738f, 0.666f, -0.574f, -1.003f, 5.687f, 0.000f, 0.000f, unlimited };
        constexpr Voicing tubeVoicing  {  5.444f, 0.050f, -0.122f,  0.119f, 1.752f, 1.957f, 0.871f, 0.413f };
        constexpr Voicing heavyVoicing {  5.582f, 0.200f, -0.925f, -0.066f, 1.785f, 0.000f, 0.918f, 0.662f };
        constexpr Voicing edgeVoicing  {  8.008f, 0.552f, -1.135f, -0.448f, 7.999f, 0.000f, 0.000f, unlimited };
        // Grit was refitted (2026-11-27) to maps of every harmonic up to the 40th against
        // Drive, from P recorded in Logic (44 / 175 / 700 Hz notes, Punish off and on, about
        // -13 dBFS peak). Its lopsidedness limit grows with Drive (0.21 -> 0.91 of the signal
        // peak) so the tone keeps changing as Drive rises instead of freezing, giving moving
        // harmonic gaps like P's; owner picked it by ear over the previous fit and a hard-ceiling design.
        constexpr Voicing gritVoicing  { -0.971f, -0.802f, 1.832f, 1.531f, 1.726f, 0.279f, 0.479f, 0.210f, 0.702f, -0.402f };

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
        const auto driveFraction = std::clamp (drive.driveDb / maxDriveDb, 0.0f, 1.0f);

        CurveSettings settings;
        settings.inputGain = decibelsToGain (v.sensitivityDb);
        settings.bias = biasFor (v, drive);
        settings.hardness = v.hardness + v.hardnessAcrossDrive * driveFraction + v.hardnessFromPush * drive.push;
        settings.biasLimitRatio = v.biasLimitRatio + v.biasLimitAcrossDrive * driveFraction;
        return settings;
    }
}
