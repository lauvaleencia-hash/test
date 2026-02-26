#pragma once

#include <JuceHeader.h>

struct ModulationSnapshot
{
    float lfo1 { 0.0f };
    float lfo2 { 0.0f };
    float lfo3 { 0.0f };
    float envelopeA { 0.0f };
    float envelopeB { 0.0f };
    float chaos { 0.0f };
};

class ModulationEngine
{
public:
    void prepare (double sr)
    {
        sampleRate = sr;
        envA.reset (sr, 0.02);
        envB.reset (sr, 0.08);
    }

    void setLfoRates (float r1, float r2, float r3)
    {
        rate1 = r1;
        rate2 = r2;
        rate3 = r3;
    }

    ModulationSnapshot processFrame (float monoInput)
    {
        const auto dt = 1.0f / (float) sampleRate;
        phase1 = std::fmod (phase1 + rate1 * dt, 1.0f);
        phase2 = std::fmod (phase2 + rate2 * dt, 1.0f);
        phase3 = std::fmod (phase3 + rate3 * dt, 1.0f);

        snapshot.lfo1 = std::sin (juce::MathConstants<float>::twoPi * phase1);
        snapshot.lfo2 = 2.0f * std::abs (2.0f * phase2 - 1.0f) - 1.0f;
        snapshot.lfo3 = std::sin (juce::MathConstants<float>::twoPi * phase3 + 0.5f * snapshot.lfo1);

        snapshot.envelopeA = envA.processSample (0, std::abs (monoInput));
        snapshot.envelopeB = envB.processSample (0, std::abs (monoInput));

        const auto logisticR = 3.78f + 0.1f * snapshot.envelopeA;
        chaosState = logisticR * chaosState * (1.0f - chaosState);
        snapshot.chaos = 2.0f * chaosState - 1.0f;
        return snapshot;
    }

private:
    double sampleRate { 44100.0 };
    float phase1 { 0.0f }, phase2 { 0.0f }, phase3 { 0.0f };
    float rate1 { 0.2f }, rate2 { 0.37f }, rate3 { 0.93f };
    float chaosState { 0.5f };

    juce::dsp::BallisticsFilter<float> envA, envB;
    ModulationSnapshot snapshot;
};
