#pragma once

#include "CharacterShaper.h"
#include "StereoSpread.h"

#include <juce_dsp/juce_dsp.h>

namespace rubbish
{
    /** The core distortion: drive gain, then the Character curve run at a higher
        internal sample rate (oversampled) so harsh harmonics don't fold back as
        digital fizz, with Spread working on the stereo sides, then removal of any DC offset the curve created. */
    class DistortionStage
    {
    public:
        void prepare (const juce::dsp::ProcessSpec& spec);
        void reset();

        /** Drive and Push. Push's extra gain is already included in totalDb. */
        void setDrive (const DriveState& newDrive) noexcept;
        void setCharacter (Character newCharacter) noexcept;
        void setSpread (float newSpread) noexcept; // 0..1

        void process (juce::dsp::AudioBlock<float>& block) noexcept;

        int getLatencyInSamples() const noexcept;

    private:
        void processChunk (juce::dsp::AudioBlock<float>& chunk) noexcept;
        void prepareDcBlocker (const juce::dsp::ProcessSpec& spec);

        using DcBlocker = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                                         juce::dsp::IIR::Coefficients<float>>;

        static constexpr size_t oversamplingStages = 2; // 2 stages of 2x = 4x
        static constexpr double driveRampSeconds = 0.05;
        static constexpr float dcBlockerHz = 10.0f;

        std::unique_ptr<juce::dsp::Oversampling<float>> oversampling;
        juce::dsp::Gain<float> driveGain;
        CharacterShaper shaper;
        StereoSpread spread;
        DcBlocker dcBlocker;
        size_t maxChunkSize = 0;
    };
}
