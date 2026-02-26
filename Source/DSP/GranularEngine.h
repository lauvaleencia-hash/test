#pragma once

#include <JuceHeader.h>

class GranularEngine
{
public:
    void prepare (double newSampleRate, int maximumBlockSize)
    {
        sampleRate = newSampleRate;
        writePosition = 0;
        const auto size = juce::jmax (maximumBlockSize * 16, 1 << 16);
        delayBuffer.setSize (2, size);
        delayBuffer.clear();
    }

    void setParameters (float grainSizeMs, float density, float jitter, float scrub, float freezeValue, float pitch)
    {
        grainSizeSamples = juce::jmax (1.0f, grainSizeMs * 0.001f * (float) sampleRate);
        grainDensity = density;
        grainJitter = jitter;
        scrubPosition = scrub;
        freeze = freezeValue > 0.5f;
        pitchRatio = juce::jlimit (0.5f, 2.0f, pitch);
    }

    void process (juce::AudioBuffer<float>& buffer)
    {
        jassert (delayBuffer.getNumSamples() > 0);

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* out = buffer.getWritePointer (ch);
            auto* ring = delayBuffer.getWritePointer (juce::jmin (ch, delayBuffer.getNumChannels() - 1));
            const auto ringSize = delayBuffer.getNumSamples();

            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                if (! freeze)
                    ring[(writePosition + i) % ringSize] = out[i];

                const auto randOffset = (random.nextFloat() - 0.5f) * grainJitter * grainSizeSamples;
                const auto scrubOffset = scrubPosition * (float) ringSize;
                const auto baseRead = (float) (writePosition + i) - scrubOffset - grainSizeSamples - randOffset;
                auto readPosition = baseRead * pitchRatio;

                while (readPosition < 0.0f)
                    readPosition += (float) ringSize;
                while (readPosition >= (float) ringSize)
                    readPosition -= (float) ringSize;

                const auto idxA = (int) readPosition;
                const auto idxB = (idxA + 1) % ringSize;
                const auto frac = readPosition - (float) idxA;
                const auto grain = ring[idxA] + frac * (ring[idxB] - ring[idxA]);

                const auto densityMix = juce::jlimit (0.0f, 1.0f, grainDensity);
                out[i] = juce::jmap (densityMix, out[i], grain);
            }
        }

        writePosition = (writePosition + buffer.getNumSamples()) % delayBuffer.getNumSamples();
    }

private:
    double sampleRate { 44100.0 };
    juce::AudioBuffer<float> delayBuffer;
    int writePosition { 0 };

    float grainSizeSamples { 2048.0f };
    float grainDensity { 0.5f };
    float grainJitter { 0.0f };
    float scrubPosition { 0.0f };
    bool freeze { false };
    float pitchRatio { 1.0f };

    juce::Random random;
};
