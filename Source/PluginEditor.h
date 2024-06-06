/*
  ==============================================================================

    PluginEditor.h
    Created: 26 Feb 2024 1:17:36pm
    Author:  matth

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WaveThumbnail.h"
#include "ADSRComponent.h"
#include "SampleSelectorComponent.h"

//==============================================================================
/**
*/
class SammyAudioProcessorEditor : public juce::AudioProcessorEditor,
    public Timer
{
public:
    SammyAudioProcessorEditor(SammyAudioProcessor&);
    ~SammyAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;

    void setColours();

private:
    WaveThumbnail mWaveThumbnail;
    ADSRComponent mADSR;
    SampleSelectorComponent mSampleSelector;

    Slider mPitchSlider;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> mPitchAttachment;

    Colour& bgColour;
    Colour& midColour;
    Colour& darkColour;
    Colour& modColour;
    Colour& modulatorColour;

    SammyAudioProcessor& processor;

    void updateUIForSample(int sampleIndex);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SammyAudioProcessorEditor)
};
