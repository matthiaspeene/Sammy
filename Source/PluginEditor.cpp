/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SammyAudioProcessorEditor::SammyAudioProcessorEditor (SammyAudioProcessor& p)
    : AudioProcessorEditor (&p), mWaveThumbnail(p), mADSR(p), processor(p),
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

    mPitchAttachment = std::make_unique <AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "PITCH OFFSET", mPitchSlider);


    startTimerHz(30);




    setColours();
    setSize (900, 600);
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
}

void SammyAudioProcessorEditor::resized()
{
    float relativeX{ 12.0f / getWidth() };
    float relativeY{ 12.0f / getHeight() };

    float halfHeight{ 282.f / 600.f };
    float thirdWidht{ 284.0f / 900.0f };

    mWaveThumbnail.setBoundsRelative(relativeX, relativeY, 1.0f - 2 * relativeX, halfHeight);
    mADSR.setBoundsRelative(relativeX, halfHeight + 2 * relativeY, thirdWidht, halfHeight/2 - relativeY);

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


