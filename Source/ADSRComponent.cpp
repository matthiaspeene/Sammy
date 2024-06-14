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
ADSRComponent::ADSRComponent(SammyAudioProcessor& p)
    : bgColour(p.getBgColour()),
    midColour(p.getMidColour()),
    darkColour(p.getDarkColour()),
    modColour(p.getModColour()),
    modulatorColour(p.getModulatorColour()),
    processor(p)
{
    // Attack Slider
    mAttackSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mAttackSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 20);
    mAttackSlider.setColour(Slider::ColourIds::thumbColourId, modColour);
    mAttackSlider.setColour(Slider::ColourIds::rotarySliderFillColourId, modColour);
    mAttackSlider.setColour(Slider::ColourIds::rotarySliderOutlineColourId, midColour);
    mAttackSlider.setColour(Slider::ColourIds::textBoxOutlineColourId, darkColour);
    mAttackSlider.setColour(Slider::ColourIds::textBoxTextColourId, darkColour);
    mAttackSlider.setColour(Slider::ColourIds::textBoxHighlightColourId, modColour);
    mAttackSlider.addListener(this);
    addAndMakeVisible(mAttackSlider);

    mAttackLabel.setFont(12.0f);
    mAttackLabel.setText("Attack", NotificationType::dontSendNotification);
    mAttackLabel.setColour(Label::ColourIds::textColourId, darkColour);
    mAttackLabel.setJustificationType(Justification::centredBottom);
    mAttackLabel.attachToComponent(&mAttackSlider, false);

    mAttackAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "ATTACK", mAttackSlider);

    // Decay Slider
    mDecaySlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mDecaySlider.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 20);
    mDecaySlider.setColour(Slider::ColourIds::thumbColourId, modColour);
    mDecaySlider.setColour(Slider::ColourIds::rotarySliderFillColourId, modColour);
    mDecaySlider.setColour(Slider::ColourIds::rotarySliderOutlineColourId, midColour);
    mDecaySlider.setColour(Slider::ColourIds::textBoxOutlineColourId, darkColour);
    mDecaySlider.setColour(Slider::ColourIds::textBoxTextColourId, darkColour);
    mDecaySlider.setColour(Slider::ColourIds::textBoxHighlightColourId, modColour);
    mDecaySlider.addListener(this);
    addAndMakeVisible(mDecaySlider);

    mDecayLabel.setFont(12.0f);
    mDecayLabel.setText("Decay", NotificationType::dontSendNotification);
    mDecayLabel.setColour(Label::ColourIds::textColourId, darkColour);
    mDecayLabel.setJustificationType(Justification::centredBottom);
    mDecayLabel.attachToComponent(&mDecaySlider, false);

    mDecayAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "DECAY", mDecaySlider);

    // Sustain Slider
    mSustainSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mSustainSlider.setColour(Slider::ColourIds::thumbColourId, modColour);
    mSustainSlider.setColour(Slider::ColourIds::rotarySliderFillColourId, modColour);
    mSustainSlider.setColour(Slider::ColourIds::rotarySliderOutlineColourId, midColour);
    mSustainSlider.setColour(Slider::ColourIds::textBoxOutlineColourId, darkColour);
    mSustainSlider.setColour(Slider::ColourIds::textBoxTextColourId, darkColour);
    mSustainSlider.setColour(Slider::ColourIds::textBoxHighlightColourId, modColour);
    mSustainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 20);
    mSustainSlider.addListener(this);
    addAndMakeVisible(mSustainSlider);

    mSustainLabel.setFont(12.0f);
    mSustainLabel.setText("Sustain", NotificationType::dontSendNotification);
    mSustainLabel.setColour(Label::ColourIds::textColourId, darkColour);
    mSustainLabel.setJustificationType(Justification::centredBottom);
    mSustainLabel.attachToComponent(&mSustainSlider, false);

    mSustainAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "SUSTAIN", mSustainSlider);

    // Release Slider
    mReleaseSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mReleaseSlider.setColour(Slider::ColourIds::thumbColourId, modColour);
    mReleaseSlider.setColour(Slider::ColourIds::rotarySliderFillColourId, modColour);
    mReleaseSlider.setColour(Slider::ColourIds::rotarySliderOutlineColourId, midColour);
    mReleaseSlider.setColour(Slider::ColourIds::textBoxOutlineColourId, darkColour);
    mReleaseSlider.setColour(Slider::ColourIds::textBoxTextColourId, darkColour);
    mReleaseSlider.setColour(Slider::ColourIds::textBoxHighlightColourId, modColour);
    mReleaseSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 20);
    mReleaseSlider.addListener(this);
    addAndMakeVisible(mReleaseSlider);

    mReleaseLabel.setFont(12.0f);
    mReleaseLabel.setText("Release", NotificationType::dontSendNotification);
    mReleaseLabel.setColour(Label::ColourIds::textColourId, darkColour);
    mReleaseLabel.setJustificationType(Justification::centredBottom);
    mReleaseLabel.attachToComponent(&mReleaseSlider, false);

    mReleaseAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "RELEASE", mReleaseSlider);
}

ADSRComponent::~ADSRComponent()
{
}

void ADSRComponent::paint(juce::Graphics& g)
{
    g.fillAll(bgColour);
}

void ADSRComponent::resized()
{
    const auto startY = 0.15f;
    const auto startX = 0.0f;
    const auto offset = 0.25f;
    const auto dialWidth = 0.25f;
    const auto dialHeight = 1.0f - startY - 12.0f / getWidth();

    mAttackSlider.setBoundsRelative(startX, startY, dialWidth, dialHeight);
    mDecaySlider.setBoundsRelative(startX + offset, startY, dialWidth, dialHeight);
    mSustainSlider.setBoundsRelative(startX + 2 * offset, startY, dialWidth, dialHeight);
    mReleaseSlider.setBoundsRelative(startX + 3 * offset, startY, dialWidth, dialHeight);
}

void ADSRComponent::sliderValueChanged(Slider* slider)
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

void ADSRComponent::updateSettings()
{
    const auto& adsrParams = processor.getADSRParams();

    mAttackSlider.setValue(adsrParams.attack, juce::dontSendNotification);
    mDecaySlider.setValue(adsrParams.decay, juce::dontSendNotification);
    mSustainSlider.setValue(adsrParams.sustain, juce::dontSendNotification);
    mReleaseSlider.setValue(adsrParams.release, juce::dontSendNotification);
}


void ADSRComponent::setColours(Colour& bg, Colour& mid, Colour& dark, Colour& mod, Colour& modulator)
{
    bgColour = bg;
    midColour = mid;
    darkColour = dark;
    modColour = mod;
    modulatorColour = modulator;
}
