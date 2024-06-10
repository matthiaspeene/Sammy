/*
  ==============================================================================

    ADSRComponent.h
    Created: 26 Feb 2024 1:17:36pm
    Author:  matth

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class ADSRComponent : public juce::Component,
    public juce::Slider::Listener
{
public:
    ADSRComponent(SammyAudioProcessor& p);
    ~ADSRComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void setSampleIndex(int index);
    void updateSettings(int sampleIndex);

    void setColours(juce::Colour& bg, juce::Colour& mid, juce::Colour& dark, juce::Colour& mod, juce::Colour& modulator);

private:
    juce::Slider mAttackSlider, mDecaySlider, mSustainSlider, mReleaseSlider;
    juce::Label mAttackLabel, mDecayLabel, mSustainLabel, mReleaseLabel;

    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> mAttackAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> mDecayAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> mSustainAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> mReleaseAttachment;

    juce::Colour& bgColour;
    juce::Colour& midColour;
    juce::Colour& darkColour;
    juce::Colour& modColour;
    juce::Colour& modulatorColour;

    SammyAudioProcessor& processor;
    int sampleIndex{ 0 };

    void sliderValueChanged(juce::Slider* slider) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ADSRComponent)
};
