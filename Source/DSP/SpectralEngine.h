#pragma once

#include <JuceHeader.h>

class SpectralEngine
{
public:
    static constexpr int fftOrder = 11;
    static constexpr int fftSize = 1 << fftOrder;

    void prepare (int channels)
    {
        fifo.setSize (channels, fftSize);
        outputAccum.setSize (channels, fftSize);
        fifo.clear();
        outputAccum.clear();
        fifoIndex = 0;
        fftData.assign ((size_t) fftSize * 2, 0.0f);
    }

    void setParameters (float blur, float stretch, float bloom, float harmonicShift)
    {
        spectralBlur = blur;
        spectralStretch = juce::jlimit (0.5f, 2.0f, stretch);
        harmonicBloom = bloom;
        shift = harmonicShift;
    }

    void process (juce::AudioBuffer<float>& buffer)
    {
        if (buffer.getNumSamples() == 0)
            return;

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            for (int ch = 0; ch < juce::jmin (buffer.getNumChannels(), fifo.getNumChannels()); ++ch)
                fifo.setSample (ch, fifoIndex, buffer.getSample (ch, sample));

            ++fifoIndex;
            if (fifoIndex >= fftSize)
            {
                for (int ch = 0; ch < juce::jmin (buffer.getNumChannels(), fifo.getNumChannels()); ++ch)
                    runFrame (fifo.getReadPointer (ch), outputAccum.getWritePointer (ch));
                fifoIndex = 0;
            }

            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                const auto spectralSample = outputAccum.getSample (juce::jmin (ch, outputAccum.getNumChannels() - 1), fifoIndex);
                buffer.setSample (ch, sample, spectralSample);
            }
        }
    }

private:
    void runFrame (const float* in, float* out)
    {
        std::fill (fftData.begin(), fftData.end(), 0.0f);
        std::copy (in, in + fftSize, fftData.begin());
        fft.performRealOnlyForwardTransform (fftData.data());

        for (int bin = 1; bin < fftSize / 2; ++bin)
        {
            const auto i = bin * 2;
            auto real = fftData[(size_t) i];
            auto imag = fftData[(size_t) i + 1];

            const auto mag = std::sqrt (real * real + imag * imag);
            const auto phase = std::atan2 (imag, real);
            const auto bloomGain = 1.0f + harmonicBloom * std::exp (-0.002f * (float) bin);
            const auto shiftedBin = juce::jlimit (1, fftSize / 2 - 1, (int) std::round ((float) bin * spectralStretch + shift * 12.0f));
            const auto si = shiftedBin * 2;

            fftData[(size_t) si] += mag * bloomGain * std::cos (phase);
            fftData[(size_t) si + 1] += mag * bloomGain * std::sin (phase);

            const auto blurMix = spectralBlur * 0.5f;
            fftData[(size_t) i] = juce::jmap (blurMix, real, fftData[(size_t) si]);
            fftData[(size_t) i + 1] = juce::jmap (blurMix, imag, fftData[(size_t) si + 1]);
        }

        fft.performRealOnlyInverseTransform (fftData.data());
        for (int i = 0; i < fftSize; ++i)
            out[i] = fftData[(size_t) i] / (float) fftSize;
    }

    juce::dsp::FFT fft { fftOrder };
    juce::AudioBuffer<float> fifo, outputAccum;
    std::vector<float> fftData;
    int fifoIndex { 0 };

    float spectralBlur { 0.0f };
    float spectralStretch { 1.0f };
    float harmonicBloom { 0.0f };
    float shift { 0.0f };
};
