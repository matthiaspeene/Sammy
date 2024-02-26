/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SammyAudioProcessorEditor::SammyAudioProcessorEditor (SammyAudioProcessor& p)
    : AudioProcessorEditor (&p), mWaveThumbnail(p), mADSR(p), processor(p)
{
    addAndMakeVisible(mWaveThumbnail);
    addAndMakeVisible(mADSR);

    setSize (800, 300);
}

SammyAudioProcessorEditor::~SammyAudioProcessorEditor()
{
}

//==============================================================================
void SammyAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colours::black);
}

void SammyAudioProcessorEditor::resized()
{
    mWaveThumbnail.setBoundsRelative(0.0f, 0.25f, 1.0f, 0.5f);
    mADSR.setBounds(200.0f, 200.0f, 200.0f, 100.0f);
}


