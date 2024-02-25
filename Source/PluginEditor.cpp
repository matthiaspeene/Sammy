/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SammyAudioProcessorEditor::SammyAudioProcessorEditor (SammyAudioProcessor& p)
    : AudioProcessorEditor (&p), mWaveThumbnail(p), processor(p)
{
    mLoadButton.onClick = [&]() { processor.loadFile(); };
    addAndMakeVisible(mLoadButton);

    //Sliders
    {
        //Attack Slider
        mAttackSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
        mAttackSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 40, 20);
        addAndMakeVisible(mAttackSlider);

        mAttackLabel.setFont(10.0f);
        mAttackLabel.setText("Attack", NotificationType::dontSendNotification);
        mAttackLabel.setJustificationType(Justification::centredTop);
        mAttackLabel.attachToComponent(&mAttackSlider, false);

        mAttackAttachment = std::make_unique <AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "ATTACK", mAttackSlider);

        //Decay Slider
        mDecaySlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
        mDecaySlider.setTextBoxStyle(Slider::TextBoxBelow, true, 40, 20);
        addAndMakeVisible(mDecaySlider);

        mDecayLabel.setFont(10.0f);
        mDecayLabel.setText("Decay", NotificationType::dontSendNotification);
        mDecayLabel.setJustificationType(Justification::centredTop);
        mDecayLabel.attachToComponent(&mDecaySlider, false);

        mDecayAttachment = std::make_unique <AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "DECAY", mDecaySlider);

        //Sustain Slider
        mSustainSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
        mSustainSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 40, 20);
        addAndMakeVisible(mSustainSlider);

        mSustainLabel.setFont(10.0f);
        mSustainLabel.setText("Sustain", NotificationType::dontSendNotification);
        mSustainLabel.setJustificationType(Justification::centredTop);
        mSustainLabel.attachToComponent(&mSustainSlider, false);

        mSustainAttachment = std::make_unique <AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "SUSTAIN", mSustainSlider);
        
        //Release Slider
        mReleaseSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
        mReleaseSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 40, 20);
        addAndMakeVisible(mReleaseSlider);

        mReleaseLabel.setFont(10.0f);
        mReleaseLabel.setText("Release", NotificationType::dontSendNotification);
        mReleaseLabel.setJustificationType(Justification::centredTop);
        mReleaseLabel.attachToComponent(&mReleaseSlider, false);

        mReleaseAttachment = std::make_unique <AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "RELEASE", mReleaseSlider);
    }

    addAndMakeVisible(mWaveThumbnail);

    setSize (800, 300);
}

SammyAudioProcessorEditor::~SammyAudioProcessorEditor()
{
}

//==============================================================================
void SammyAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colours::black);
    g.setColour(Colours::white);


    /* Button drawing
    g.setColour(Colours::white);
    g.setFont(15.0f);

    if (processor.getNumSamlerSounds() > 0)
    {
        g.fillAll(Colours::blue);
        g.drawText("Sound Loaded", getWidth() / 2 - 50, getHeight() / 2 - 10, 100, 20, Justification::centred);
    }
    else
    {
        g.drawText("Load a Sound", getWidth() / 2 - 50, getHeight() / 2 - 10, 100, 20, Justification::centred);
    }
    */
}

void SammyAudioProcessorEditor::resized()
{
    mWaveThumbnail.setBoundsRelative(0.0f, 0.25f, 1.0f, 0.5f);

    const auto startY = 0.6f;
    const auto startX = 0.6f;
    const auto offset = 0.1f;
    const auto dialWith = 0.1f;
    const auto dialHeight = 0.4f;

    //mLoadButton.setBoundsRelative(getWidth()/ 2 - 50, getHeight()/2 -50, 100 , 100);
    mAttackSlider.setBoundsRelative(startX, startY, dialWith, dialHeight);
    mDecaySlider.setBoundsRelative(startX + offset, startY, dialWith, dialHeight);
    mSustainSlider.setBoundsRelative(startX + 2 * offset, startY, dialWith, dialHeight);
    mReleaseSlider.setBoundsRelative(startX + 3 * offset, startY, dialWith, dialHeight);
}

void SammyAudioProcessorEditor::sliderValueChange(Slider* slider)
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
