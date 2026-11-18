#pragma once

#include <juce_dsp/juce_dsp.h>

#include <vector>

namespace rubbish
{
    enum class FilterType
    {
        lowPass,
        highPass
    };

    /** A 12 dB/octave resonant filter in the spirit of the MS-20.

        It is a zero-delay state-variable filter whose resonance is held in check by
        a soft limit on its internal state. That limit is what lets high Peak values
        ring and howl without ever running away, at any input level. */
    class ResonantFilter
    {
    public:
        explicit ResonantFilter (FilterType filterType);

        void prepare (const juce::dsp::ProcessSpec& spec);
        void reset();

        void setCutoffHz (float newCutoffHz) noexcept;
        void setPeak (float newPeak) noexcept; // 0 = no resonance, 1 = maximum

        /** Optional cutoffMultipliers (one per sample) move the cutoff, e.g. for Filter Follow. */
        void process (juce::dsp::AudioBlock<float>& block, const float* cutoffMultipliers = nullptr) noexcept;

    private:
        struct Coefficients
        {
            float g = 0.0f;          // integrator gain, from cutoff
            float damping = 0.0f;    // 1 / Q, from peak
            float normaliser = 0.0f;
        };

        Coefficients nextCoefficients (float cutoffMultiplier) noexcept;
        float processSample (size_t channel, float input, const Coefficients& c) noexcept;

        static float peakToDamping (float peak) noexcept;
        static float limitState (float state) noexcept;

        static constexpr double smoothingSeconds = 0.03;

        const FilterType type;
        double sampleRate = 44100.0;
        float maxCutoffHz = 20000.0f;

        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> cutoffHz { 1000.0f };
        juce::SmoothedValue<float> peak { 0.0f };

        std::vector<float> bandState;
        std::vector<float> lowState;
    };
}
