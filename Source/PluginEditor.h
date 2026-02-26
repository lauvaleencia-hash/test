#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class FractureBloomAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                                private juce::Timer
{
public:
    explicit FractureBloomAudioProcessorEditor (FractureBloomAudioProcessor&);
    ~FractureBloomAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    juce::Slider makeDial (const juce::String& name);

    FractureBloomAudioProcessor& processor;

    juce::Slider macro, grainSize, density, blur, bloom, fracture, morph, mix, output;
    juce::Label title;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::vector<std::unique_ptr<Attachment>> attachments;

    float animationPhase { 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FractureBloomAudioProcessorEditor)
};
