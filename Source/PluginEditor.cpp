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
    : AudioProcessorEditor(&p), mWaveThumbnail(p, mSampleSelector), mADSR(p), processor(p),
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
    updateUIForSample(processor.getSelectedSampleIndex());

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
    mWaveThumbnail.setSampleIndex();
    mADSR.updateSettings();
}
