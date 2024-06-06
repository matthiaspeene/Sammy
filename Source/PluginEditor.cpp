/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SammyAudioProcessorEditor::SammyAudioProcessorEditor(SammyAudioProcessor& p)
    : AudioProcessorEditor(&p), mWaveThumbnail(p), mADSR(p), processor(p),
    bgColour(p.getBgColour()),
    midColour(p.getMidColour()),
    darkColour(p.getDarkColour()),
    modColour(p.getModColour()),
    modulatorColour(p.getModulatorColour())
{
    addAndMakeVisible(mWaveThumbnail);
    addAndMakeVisible(mADSR);

    mPitchSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mPitchSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 20);
    mPitchSlider.setColour(Slider::ColourIds::thumbColourId, modColour);
    mPitchSlider.setColour(Slider::ColourIds::rotarySliderFillColourId, modColour);
    mPitchSlider.setColour(Slider::ColourIds::rotarySliderOutlineColourId, midColour);
    mPitchSlider.setColour(Slider::ColourIds::textBoxOutlineColourId, darkColour);
    mPitchSlider.setColour(Slider::ColourIds::textBoxTextColourId, darkColour);
    mPitchSlider.setColour(Slider::ColourIds::textBoxHighlightColourId, modColour);
    addAndMakeVisible(mPitchSlider);

    mPitchAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "PITCH OFFSET", mPitchSlider);

    addAndMakeVisible(sampleSelector);
    sampleSelector.onChange = [this] { updateSampleSettingsUI(); };

    addAndMakeVisible(loadButton);
    loadButton.onClick = [this] { loadSample(); };

    startTimerHz(30);

    setColours();
    setSize(900, 600);
}

SammyAudioProcessorEditor::~SammyAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void SammyAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(midColour);
}

void SammyAudioProcessorEditor::resized()
{
    float relativeX{ 12.0f / getWidth() };
    float relativeY{ 12.0f / getHeight() };

    float halfHeight{ 282.f / 600.f };
    float thirdWidth{ 284.0f / 900.0f };

    mWaveThumbnail.setBoundsRelative(relativeX, relativeY, 1.0f - 2 * relativeX, halfHeight);
    mADSR.setBoundsRelative(relativeX, halfHeight + 2 * relativeY, thirdWidth, halfHeight / 2 - relativeY);

    mPitchSlider.setBoundsRelative(0.5, 0.5, 80, 80);

    sampleSelector.setBoundsRelative(0.5f, 0.1f, 0.4f, 0.1f);
    loadButton.setBoundsRelative(0.5f, 0.2f, 0.4f, 0.1f);
}

void SammyAudioProcessorEditor::timerCallback()
{
    repaint();
}

void SammyAudioProcessorEditor::setColours()
{
    mWaveThumbnail.setColours(bgColour, midColour, darkColour, modColour, modulatorColour);
}

void SammyAudioProcessorEditor::loadSample()
{
    processor.loadFile();
    sampleSelector.addItem("Sample " + juce::String(processor.getNumSamplerSounds()), processor.getNumSamplerSounds());
    sampleSelector.setSelectedId(processor.getNumSamplerSounds());
}

void SammyAudioProcessorEditor::updateSampleSettingsUI()
{
    int selectedSample = sampleSelector.getSelectedId() - 1;
    if (selectedSample >= 0)
    {
        processor.selectSample(selectedSample);
        mWaveThumbnail.setSampleIndex(selectedSample);
        mADSR.setSampleIndex(selectedSample);
    }
}
