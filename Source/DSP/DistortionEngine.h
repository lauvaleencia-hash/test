#pragma once

#include <JuceHeader.h>

class DistortionEngine
{
public:
    void prepare (double sr)
    {
        sampleRate = sr;
        envelopeFilter.reset (sr, 0.03);
    }

    void setParameters (float wavefold, float saturation, float envDrive)
    {
        foldAmount = wavefold;
        satAmount = saturation;
        envelopeDrive = envDrive;
    }

    float processSample (float x)
    {
        const auto env = envelopeFilter.processSample (0, std::abs (x));
        const auto dynamicDrive = 1.0f + env * envelopeDrive * 6.0f;

        auto y = std::sin (x * (1.0f + foldAmount * 8.0f));
        y = std::tanh (y * (1.0f + satAmount * 5.0f) * dynamicDrive);
        return y;
    }

private:
    double sampleRate { 44100.0 };
    juce::dsp::BallisticsFilter<float> envelopeFilter;
    float foldAmount { 0.0f };
    float satAmount { 0.0f };
    float envelopeDrive { 0.0f };
};
