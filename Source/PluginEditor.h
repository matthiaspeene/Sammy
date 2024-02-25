/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SammyAudioProcessorEditor : public juce::AudioProcessorEditor,
                                    public FileDragAndDropTarget,
                                    public Slider::Listener
{
public:
    SammyAudioProcessorEditor (SammyAudioProcessor&);
    ~SammyAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    // File Drag and drop
    bool isInterestedInFileDrag(const StringArray& files) override;
    void filesDropped(const StringArray& files, int x, int y) override;

private:
    TextButton mLoadButton{ "Load" };
    Slider mAttackSlider, mDecaySlider, mSustainSlider, mReleaseSlider;
    Label mAttackLabel, mDecayLabel, mSustainLabel, mReleaseLabel;

    std::vector<float> mAudioPoints;
    bool mShouldBePainting{ false };

    //Reference to the plugin proccesor so we can link parameters and such. 
    SammyAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SammyAudioProcessorEditor)
};
