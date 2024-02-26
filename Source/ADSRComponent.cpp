/*
  ==============================================================================

    ADSRComponent.cpp
    Created: 26 Feb 2024 1:17:36pm
    Author:  matth

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ADSRComponent.h"

//==============================================================================
ADSRComponent::ADSRComponent(SammyAudioProcessor& p) : processor(p)
{
    //Attack Slider
    mAttackSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mAttackSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 20);
    addAndMakeVisible(mAttackSlider);

    mAttackLabel.setFont(10.0f);
    mAttackLabel.setText("Attack", NotificationType::dontSendNotification);
    mAttackLabel.setJustificationType(Justification::centredBottom);
    mAttackLabel.attachToComponent(&mAttackSlider, false);

    mAttackAttachment = std::make_unique <AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "ATTACK", mAttackSlider);

    //Decay Slider
    mDecaySlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mDecaySlider.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 20);
    addAndMakeVisible(mDecaySlider);

    mDecayLabel.setFont(10.0f);
    mDecayLabel.setText("Decay", NotificationType::dontSendNotification);
    mDecayLabel.setJustificationType(Justification::centredBottom);
    mDecayLabel.attachToComponent(&mDecaySlider, false);

    mDecayAttachment = std::make_unique <AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "DECAY", mDecaySlider);

    //Sustain Slider
    mSustainSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mSustainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 20);
    addAndMakeVisible(mSustainSlider);

    mSustainLabel.setFont(10.0f);
    mSustainLabel.setText("Sustain", NotificationType::dontSendNotification);
    mSustainLabel.setJustificationType(Justification::centredBottom);
    mSustainLabel.attachToComponent(&mSustainSlider, false);


    mSustainAttachment = std::make_unique <AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "SUSTAIN", mSustainSlider);

    //Release Slider
    mReleaseSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mReleaseSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 20);
    addAndMakeVisible(mReleaseSlider);

    mReleaseLabel.setFont(10.0f);
    mReleaseLabel.setText("Release", NotificationType::dontSendNotification);
    mReleaseLabel.setJustificationType(Justification::centredBottom);
    mReleaseLabel.attachToComponent(&mReleaseSlider, false);


    mReleaseAttachment = std::make_unique <AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "RELEASE", mReleaseSlider);
}

ADSRComponent::~ADSRComponent()
{
}

void ADSRComponent::paint (juce::Graphics& g)
{
    g.fillAll(Colours::burlywood);
}

void ADSRComponent::resized()
{
    const auto startY = 0.15f;
    const auto startX = 0.0f;
    const auto offset = 0.25f;
    const auto dialWith = 0.25f;
    const auto dialHeight = 1.0f - startY;

    //mLoadButton.setBoundsRelative(getWidth()/ 2 - 50, getHeight()/2 -50, 100 , 100);
    mAttackSlider.setBoundsRelative(startX, startY, dialWith, dialHeight);
    mDecaySlider.setBoundsRelative(startX + offset, startY, dialWith, dialHeight);
    mSustainSlider.setBoundsRelative(startX + 2 * offset, startY, dialWith, dialHeight);
    mReleaseSlider.setBoundsRelative(startX + 3 * offset, startY, dialWith, dialHeight);
}

void ADSRComponent::sliderValueChange(Slider* slider)
{
    if (slider == &mAttackSlider)
    {
        processor.getADSRParams().attack = mAttackSlider.getValue();
    }
    else if (slider == &mDecaySlider)
    {
        processor.getADSRParams().decay = mDecaySlider.getValue();
    }
    else if (slider == &mSustainSlider)
    {
        processor.getADSRParams().sustain = mSustainSlider.getValue();
    }
    else if (slider == &mReleaseSlider)
    {
        processor.getADSRParams().release = mReleaseSlider.getValue();
    }

    processor.updateADSR();
}