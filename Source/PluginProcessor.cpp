#include "PluginProcessor.h"
#include "PluginEditor.h"

FractureBloomAudioProcessor::FractureBloomAudioProcessor()
    : AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", createParameterLayout())
{
    macroIntensity = apvts.getRawParameterValue ("macro");
    outGain = apvts.getRawParameterValue ("output");
}

juce::AudioProcessorValueTreeState::ParameterLayout FractureBloomAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    auto norm = juce::NormalisableRange<float> (0.0f, 1.0f, 0.0001f);

    p.push_back (std::make_unique<juce::AudioParameterFloat> ("macro", "Macro Intensity", norm, 0.45f));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("grainSize", "Grain Size", juce::NormalisableRange<float> (5.0f, 300.0f, 0.1f), 65.0f));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("density", "Density", norm, 0.4f));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("jitter", "Jitter", norm, 0.2f));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("scrub", "Scrub", norm, 0.5f));
    p.push_back (std::make_unique<juce::AudioParameterBool> ("freeze", "Freeze", false));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("pitch", "Pitch Ratio", juce::NormalisableRange<float> (0.5f, 2.0f, 0.001f), 1.0f));

    p.push_back (std::make_unique<juce::AudioParameterFloat> ("blur", "Spectral Blur", norm, 0.2f));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("stretch", "Spectral Stretch", juce::NormalisableRange<float> (0.5f, 2.0f, 0.001f), 1.0f));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("bloom", "Harmonic Bloom", norm, 0.3f));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("shift", "Harmonic Shift", juce::NormalisableRange<float> (-1.0f, 1.0f, 0.001f), 0.0f));

    p.push_back (std::make_unique<juce::AudioParameterFloat> ("fold", "Wavefold", norm, 0.2f));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("sat", "Multiband Saturation", norm, 0.25f));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("envDrive", "Envelope Drive", norm, 0.35f));

    p.push_back (std::make_unique<juce::AudioParameterFloat> ("lfo1", "LFO 1 Rate", juce::NormalisableRange<float> (0.01f, 20.0f, 0.001f, 0.35f), 0.35f));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("lfo2", "LFO 2 Rate", juce::NormalisableRange<float> (0.01f, 20.0f, 0.001f, 0.35f), 0.75f));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("lfo3", "LFO 3 Rate", juce::NormalisableRange<float> (0.01f, 20.0f, 0.001f, 0.35f), 1.35f));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("fracture", "Time Fracture", norm, 0.3f));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("glitchIQ", "Glitch Intelligence", norm, 0.4f));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("morph", "Neural Morph", norm, 0.0f));

    p.push_back (std::make_unique<juce::AudioParameterFloat> ("mix", "Mix", norm, 0.6f));
    p.push_back (std::make_unique<juce::AudioParameterFloat> ("output", "Output", juce::NormalisableRange<float> (-18.0f, 18.0f, 0.01f), 0.0f));

    return { p.begin(), p.end() };
}

void FractureBloomAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    granular.prepare (sampleRate, samplesPerBlock);
    spectral.prepare (getTotalNumOutputChannels());
    distortion.prepare (sampleRate);
    modulation.prepare (sampleRate);
    setLatencySamples (spectralLatency);
}

void FractureBloomAudioProcessor::releaseResources() {}

bool FractureBloomAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet();
}

void FractureBloomAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const auto totalIn = getTotalNumInputChannels();
    const auto totalOut = getTotalNumOutputChannels();
    for (int i = totalIn; i < totalOut; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    const auto macro = macroIntensity->load();
    auto wet = buffer;

    modulation.setLfoRates (*apvts.getRawParameterValue ("lfo1"), *apvts.getRawParameterValue ("lfo2"), *apvts.getRawParameterValue ("lfo3"));
    auto snap = modulation.processFrame (buffer.getRMSLevel (0, 0, buffer.getNumSamples()));

    const auto smartJitter = juce::jlimit (0.0f, 1.0f, *apvts.getRawParameterValue ("jitter") + snap.chaos * 0.1f);
    const auto dynamicDensity = juce::jlimit (0.0f, 1.0f, *apvts.getRawParameterValue ("density") + snap.lfo1 * 0.2f * macro);

    granular.setParameters (*apvts.getRawParameterValue ("grainSize"), dynamicDensity, smartJitter,
                            *apvts.getRawParameterValue ("scrub"), *apvts.getRawParameterValue ("freeze"),
                            *apvts.getRawParameterValue ("pitch"));
    granular.process (wet);

    spectral.setParameters (*apvts.getRawParameterValue ("blur") * macro,
                            *apvts.getRawParameterValue ("stretch"),
                            *apvts.getRawParameterValue ("bloom"),
                            *apvts.getRawParameterValue ("shift") + 0.1f * snap.lfo2);
    spectral.process (wet);

    distortion.setParameters (*apvts.getRawParameterValue ("fold") * macro,
                              *apvts.getRawParameterValue ("sat"),
                              *apvts.getRawParameterValue ("envDrive"));

    for (int ch = 0; ch < wet.getNumChannels(); ++ch)
    {
        auto* d = wet.getWritePointer (ch);
        for (int i = 0; i < wet.getNumSamples(); ++i)
            d[i] = distortion.processSample (d[i]);
    }

    const auto morph = *apvts.getRawParameterValue ("morph");
    const auto mix = *apvts.getRawParameterValue ("mix");
    const auto gain = juce::Decibels::decibelsToGain (outGain->load());

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* dry = buffer.getWritePointer (ch);
        auto* processed = wet.getWritePointer (ch);

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            const auto fractured = (i % juce::jmax (1, (int) (32.0f - 28.0f * *apvts.getRawParameterValue ("fracture")))) == 0
                                       ? processed[i]
                                       : processed[juce::jmax (0, i - 1)];
            const auto neural = juce::jmap (morph, processed[i], fractured);
            const auto out = juce::jmap (mix, dry[i], neural) * gain;
            dry[i] = out;
        }
    }
}

void FractureBloomAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void FractureBloomAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorEditor* FractureBloomAudioProcessor::createEditor()
{
    return new FractureBloomAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FractureBloomAudioProcessor();
}
