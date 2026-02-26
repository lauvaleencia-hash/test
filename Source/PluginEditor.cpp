#include "PluginEditor.h"

FractureBloomAudioProcessorEditor::FractureBloomAudioProcessorEditor (FractureBloomAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setResizable (true, true);
    setResizeLimits (860, 500, 1720, 1000);
    setSize (980, 620);

    title.setText ("FRACTURE BLOOM", juce::dontSendNotification);
    title.setJustificationType (juce::Justification::centred);
    title.setFont (juce::Font (32.0f, juce::Font::bold));
    title.setColour (juce::Label::textColourId, juce::Colour (0xffe7e9ff));
    addAndMakeVisible (title);

    macro = makeDial ("Macro");
    grainSize = makeDial ("Grain");
    density = makeDial ("Density");
    blur = makeDial ("Blur");
    bloom = makeDial ("Bloom");
    fracture = makeDial ("Fracture");
    morph = makeDial ("Neural Morph");
    mix = makeDial ("Mix");
    output = makeDial ("Output");

    attachments.emplace_back (std::make_unique<Attachment> (processor.apvts, "macro", macro));
    attachments.emplace_back (std::make_unique<Attachment> (processor.apvts, "grainSize", grainSize));
    attachments.emplace_back (std::make_unique<Attachment> (processor.apvts, "density", density));
    attachments.emplace_back (std::make_unique<Attachment> (processor.apvts, "blur", blur));
    attachments.emplace_back (std::make_unique<Attachment> (processor.apvts, "bloom", bloom));
    attachments.emplace_back (std::make_unique<Attachment> (processor.apvts, "fracture", fracture));
    attachments.emplace_back (std::make_unique<Attachment> (processor.apvts, "morph", morph));
    attachments.emplace_back (std::make_unique<Attachment> (processor.apvts, "mix", mix));
    attachments.emplace_back (std::make_unique<Attachment> (processor.apvts, "output", output));

    startTimerHz (30);
}

juce::Slider FractureBloomAudioProcessorEditor::makeDial (const juce::String& name)
{
    juce::Slider s;
    s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 90, 20);
    s.setName (name);
    s.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff99ffda));
    s.setColour (juce::Slider::thumbColourId, juce::Colour (0xffc8afff));
    s.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffe6ecff));
    addAndMakeVisible (s);
    return s;
}

void FractureBloomAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    juce::ColourGradient bg (juce::Colour (0xff0d1022), 0, 0,
                             juce::Colour (0xff151033), bounds.getWidth(), bounds.getHeight(), false);
    g.setGradientFill (bg);
    g.fillAll();

    g.setColour (juce::Colour (0x20b7ffea));
    const auto waveY = bounds.getCentreY();
    juce::Path p;
    p.startNewSubPath (0.0f, waveY);
    for (int x = 0; x < getWidth(); ++x)
    {
        const auto f = (float) x / (float) getWidth();
        const auto y = waveY + 70.0f * std::sin (f * 12.0f + animationPhase) * std::exp (-0.8f * std::abs (f - 0.5f));
        p.lineTo ((float) x, y);
    }
    g.strokePath (p, juce::PathStrokeType (2.0f));
}

void FractureBloomAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (16);
    title.setBounds (area.removeFromTop (60));

    auto row = area.removeFromTop (240);
    auto place = [&row] (juce::Slider& s)
    {
        s.setBounds (row.removeFromLeft (row.getWidth() / 5).reduced (8));
    };

    place (macro); place (grainSize); place (density); place (blur); place (bloom);

    auto row2 = area.removeFromTop (240);
    auto place2 = [&row2] (juce::Slider& s)
    {
        s.setBounds (row2.removeFromLeft (row2.getWidth() / 4).reduced (8));
    };

    place2 (fracture); place2 (morph); place2 (mix); place2 (output);
}

void FractureBloomAudioProcessorEditor::timerCallback()
{
    animationPhase += 0.05f;
    repaint();
}
