/*
  ==============================================================================

    WaveThumbnail.h
    Created: 26 Feb 2024 12:22:44am
    Author:  matth

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class WaveThumbnail : public juce::Component, public juce::FileDragAndDropTarget, public juce::Slider::Listener
{
public:
    WaveThumbnail(SammyAudioProcessor& p);
    ~WaveThumbnail() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    void setSampleIndex(int index);
    void updateSettings(int index);
    void setColours(juce::Colour& bg, juce::Colour& mid, juce::Colour& dark, juce::Colour& mod, juce::Colour& modulator);

private:
    void updateWaveForm(int index);

    juce::Slider mStartPosSlider, mRandomStartSlider, mZoomSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mStartPosAttachment, mRandomStartAttachment;

    juce::Path wavePath;
    std::vector<float> mAudioPoints;

    juce::Colour& bgColour;
    juce::Colour& midColour;
    juce::Colour& darkColour;
    juce::Colour& modColour;
    juce::Colour& modulatorColour;

    int sampleIndex{ 0 };
    bool mShouldBePainting{ false };
    bool mShouldDisplayError{ false };

    void sliderValueChanged(juce::Slider* slider) override;

    SammyAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveThumbnail)
};
