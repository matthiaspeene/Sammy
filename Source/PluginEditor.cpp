/*
  ==============================================================================

    PluginEditor.cpp
    Created: 26 Feb 2024 1:17:36pm
    Author:  matth

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SammyAudioProcessorEditor::SammyAudioProcessorEditor(SammyAudioProcessor& p)
    : AudioProcessorEditor(&p), mWaveThumbnail(p), mADSR(p), processor(p),
    mSampleSelector(p),
    bgColour(p.getBgColour()),
    midColour(p.getMidColour()),
    darkColour(p.getDarkColour()),
    modColour(p.getModColour()),
    modulatorColour(p.getModulatorColour())
{
    addAndMakeVisible(mWaveThumbnail);
    addAndMakeVisible(mADSR);
    addAndMakeVisible(mSampleSelector);

    mSampleSelector.onSampleButtonClicked = [this](int index) { updateUIForSample(index); }; // Important callback. This updates the UI

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
    auto area = getLocalBounds();
    auto selectorHeight = 30;

    mSampleSelector.setBounds(area.removeFromTop(selectorHeight));
    mWaveThumbnail.setBounds(area.removeFromTop(area.getHeight() / 2));
    mADSR.setBounds(area.removeFromLeft(area.getWidth() / 2));
    mPitchSlider.setBoundsRelative(0.5, 0.5, 80, 80);
}

void SammyAudioProcessorEditor::timerCallback()
{
    repaint();
}

void SammyAudioProcessorEditor::setColours()
{
    mWaveThumbnail.setColours(bgColour, midColour, darkColour, modColour, modulatorColour);
}

void SammyAudioProcessorEditor::updateUIForSample(int sampleIndex)
{
    processor.selectSample(sampleIndex);
    mWaveThumbnail.setSampleIndex(sampleIndex);
    mADSR.setSampleIndex(sampleIndex);
}
