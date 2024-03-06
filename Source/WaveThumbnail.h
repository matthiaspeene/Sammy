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
class WaveThumbnail  : public juce::Component,
                       public FileDragAndDropTarget
{
public:
    WaveThumbnail(SammyAudioProcessor& p);
    ~WaveThumbnail() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    bool isInterestedInFileDrag(const StringArray& files) override;
    void filesDropped(const StringArray& files, int x, int y) override;

    //void sliderValueChange(Slider* slider);

    void setColours(Colour& bg, Colour& mid, Colour& dark, Colour& mod, Colour& modulator);

private:
    std::vector<float> mAudioPoints;

    bool mShouldBePainting{ false };
    bool mShouldDisplayError{ false };

    Slider mStartPosSlider;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> mStartPosAttachment;

    Colour& bgColour;
    Colour& midColour;
    Colour& darkColour;
    Colour& modColour;
    Colour& modulatorColour;

    SammyAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveThumbnail)
};
