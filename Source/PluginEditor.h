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
class SammyAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    SammyAudioProcessorEditor (SammyAudioProcessor&);
    ~SammyAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    WaveThumbnail mWaveThumbnail;
    ADSRComponent mADSR;


    SammyAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SammyAudioProcessorEditor)
};
