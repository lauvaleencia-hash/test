#pragma once

#include <JuceHeader.h>
#include "DSP/GranularEngine.h"
#include "DSP/SpectralEngine.h"
#include "DSP/DistortionEngine.h"
#include "DSP/ModulationEngine.h"

class FractureBloomAudioProcessor final : public juce::AudioProcessor
{
public:
    FractureBloomAudioProcessor();
    ~FractureBloomAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.5; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    int getLatencySamples() const override { return spectralLatency; }

    juce::AudioProcessorValueTreeState apvts;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    GranularEngine granular;
    SpectralEngine spectral;
    DistortionEngine distortion;
    ModulationEngine modulation;

    std::atomic<float>* macroIntensity { nullptr };
    std::atomic<float>* outGain { nullptr };

    int spectralLatency { SpectralEngine::fftSize / 2 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FractureBloomAudioProcessor)
};

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();
