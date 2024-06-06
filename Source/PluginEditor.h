/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WaveThumbnail.h"
#include "ADSRComponent.h"

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

    Slider mPitchSlider;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> mPitchAttachment;

    Colour& bgColour;
    Colour& midColour;
    Colour& darkColour;
    Colour& modColour;
    Colour& modulatorColour;

    SammyAudioProcessor& processor;

    juce::ComboBox sampleSelector;
    juce::TextButton loadButton{ "Load Sample" };

    void loadSample();
    void updateSampleSettingsUI();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SammyAudioProcessorEditor)
};
