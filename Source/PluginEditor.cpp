/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SammyAudioProcessorEditor::SammyAudioProcessorEditor (SammyAudioProcessor& p)
    : AudioProcessorEditor (&p), mWaveThumbnail(p), mADSR{ p, p, p, p, p, p, p, p, p, p, p, p }, mIndexTab(p), processor(p),
    bgColour(p.getBgColour()),
    midColour(p.getMidColour()),
    darkColour(p.getDarkColour()),
    modColour(p.getModColour()),
    modulatorColour(p.getModulatorColour())
{
    addAndMakeVisible(mWaveThumbnail);

    for (int i = 0; i < 12; i++)
    {
        addAndMakeVisible(mADSR[i]);
    }

    addAndMakeVisible(mIndexTab);

    startTimerHz(30);




    setColours();
    setSize (900, 656);
}

SammyAudioProcessorEditor::~SammyAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void SammyAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)

    g.fillAll(midColour);

    for (int i = 0; i < 12; i++)
    {
        mADSR[i].setAlwaysOnTop(false);
    }

    mADSR[processor.getSampleIndex()].setAlwaysOnTop(true);

}

void SammyAudioProcessorEditor::resized()
{
    float relativeX{ 12.0f / getWidth() };
    float relativeY{ 12.0f / getHeight() };

    float halfHeight{ 310.f / getHeight() };
    float thirdWidht{ 284.0f / getWidth() };

    float topButtonHeight{ 64.f / getHeight() };

    mIndexTab.setBoundsRelative(relativeX, relativeY, 1.f - relativeX, topButtonHeight);
    mWaveThumbnail.setBoundsRelative(relativeX, relativeY + topButtonHeight, 1.f - relativeX, halfHeight - topButtonHeight);
    mADSR[processor.getSampleIndex()].setBoundsRelative(relativeX, halfHeight + 2 * relativeY, thirdWidht, halfHeight / 2 - relativeY / 2);
}

void SammyAudioProcessorEditor::timerCallback()
{
    repaint();
}

void SammyAudioProcessorEditor::setColours()
{
    mWaveThumbnail.setColours(bgColour, midColour, darkColour, modColour, modulatorColour);
}


