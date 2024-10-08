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
#include "MIDINoteRangeComponent.h"
#include "Noisy.h"
#include "FilterComponent.h"
#include "PitchControll.h"
#include "SimpleCompression.h"
#include "SimpleDistortion.h"
#include "SimpleEQ.h"

//==============================================================================
/**
*/
class SammyAudioProcessorEditor : public juce::AudioProcessorEditor,
    public Timer
{
public:
    SammyAudioProcessorEditor(SammyAudioProcessor& p);
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
    MIDINoteRangeComponent mMidiNoteRangeComponent;
    Noisy mNoisyComponent;
    FilterComponent mFilterComponent;
    PitchControll mPitchControll;
    SimpleCompression mSimpleCompression;
    SimpleDistortion mSimpleDistortion;
    SimpleEQ mSimpleEQ;

    Colour& bgColour;
    Colour& midColour;
    Colour& darkColour;
    Colour& modColour;
    Colour& modulatorColour;

    SammyAudioProcessor& processor;

    void updateUIForSample(int sampleIndex);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SammyAudioProcessorEditor)
};
