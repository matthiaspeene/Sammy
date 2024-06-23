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
    : AudioProcessorEditor(&p), mWaveThumbnail(p, mSampleSelector), mADSR(p), mMidiNoteRangeComponent(p), processor(p), mSampleSelector(p),
    mFilterComponent(p, p.getBgColour(),p.getMidColour(), p.getDarkColour(), p.getModColour(), p.getModulatorColour()), 
    mNoisyComponent(p, p.getBgColour(), p.getMidColour(), p.getDarkColour(), p.getModColour(), p.getModulatorColour()), 
    mSimpleEQ(p, p.getBgColour(),p.getMidColour(), p.getDarkColour(), p.getModColour(), p.getModulatorColour()), 
    mSimpleDistortion(p, p.getBgColour(), p.getMidColour(), p.getDarkColour(), p.getModColour(), p.getModulatorColour()),
    mSimpleCompression(p, p.getBgColour(), p.getMidColour(), p.getDarkColour(), p.getModColour(), p.getModulatorColour()), 
    mPitchControll(p, p.getBgColour(), p.getMidColour(), p.getDarkColour(), p.getModColour(), p.getModulatorColour()),

    bgColour(p.getBgColour()),
    midColour(p.getMidColour()),
    darkColour(p.getDarkColour()),
    modColour(p.getModColour()),
    modulatorColour(p.getModulatorColour())
{
    addAndMakeVisible(mWaveThumbnail);
    addAndMakeVisible(mADSR);
    addAndMakeVisible(mSampleSelector);
    addAndMakeVisible(mMidiNoteRangeComponent);
    addAndMakeVisible(mFilterComponent);
    addAndMakeVisible(mNoisyComponent);
    addAndMakeVisible(mSimpleCompression);
    addAndMakeVisible(mSimpleDistortion);
    addAndMakeVisible(mSimpleEQ);
    addAndMakeVisible(mPitchControll);

    mSampleSelector.onSampleButtonClicked = [this](int index) { updateUIForSample(index); }; // Important callback. This updates the UI
    
    updateUIForSample(processor.getSelectedSampleIndex());

    startTimerHz(30);

    setColours();
    setSize(1000, 700);
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
    auto insetSize = 12;
    auto area = getLocalBounds().reduced(insetSize);
    auto selectorHeight = insetSize * 3;

    mSampleSelector.setBounds(area.removeFromTop(selectorHeight));
    mWaveThumbnail.setBounds(area.removeFromTop(area.getHeight() / 2));
    area.removeFromTop(insetSize);

    // Bottom (prep)
    int segments = 3;
    auto segmentLeft = area.removeFromLeft((area.getWidth() / segments) - (insetSize / segments));
    area.removeFromLeft(insetSize);
    segments--;
    auto segmentMiddle = area.removeFromLeft((area.getWidth() / segments) - (insetSize / segments));
    area.removeFromLeft(insetSize);
    auto segmentRight = area;

    //BottomLeft
    segments = 2;
    mADSR.setBounds(segmentLeft.removeFromTop(segmentLeft.getHeight()/segments - insetSize/segments));
    segmentLeft.removeFromTop(insetSize);
    mPitchControll.setBounds(segmentLeft);

    // BottomMiddle
    mFilterComponent.setBounds(segmentMiddle.removeFromTop(segmentMiddle.getHeight() / segments - insetSize / segments));
    segmentMiddle.removeFromTop(insetSize);
    mMidiNoteRangeComponent.setBounds(segmentMiddle);

    //BottomRight
    segments = 3;
    mNoisyComponent.setBounds(segmentRight.removeFromTop(segmentRight.getHeight() / segments - insetSize / segments));
    segmentRight.removeFromTop(insetSize);
    segments--;
    mSimpleEQ.setBounds(segmentRight.removeFromTop(segmentRight.getHeight() / segments - insetSize / segments));
    segmentRight.removeFromTop(insetSize);

    segments = 2;
    mSimpleDistortion.setBounds(segmentRight.removeFromLeft((segmentRight.getWidth() / segments) - (insetSize / segments)));
    segmentRight.removeFromLeft(insetSize);
    mSimpleCompression.setBounds(segmentRight);
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
    mWaveThumbnail.updateSettings();
    mADSR.updateSettings();
    mMidiNoteRangeComponent.updateSettings();
}
