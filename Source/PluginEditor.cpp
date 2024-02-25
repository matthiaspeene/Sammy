/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SammyAudioProcessorEditor::SammyAudioProcessorEditor (SammyAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    mLoadButton.onClick = [&]() { processor.loadFile(); };
    addAndMakeVisible(mLoadButton);

    //Sliders
    {
        //Attack Slider
        mAttackSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
        mAttackSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 40, 20);
        mAttackSlider.setRange(0.0f, 20.0f, 0.01f);
        mAttackSlider.setSkewFactor(0.5f, false);
        addAndMakeVisible(mAttackSlider);

        mAttackLabel.setFont(10.0f);
        mAttackLabel.setText("Attack", NotificationType::dontSendNotification);
        mAttackLabel.setJustificationType(Justification::centredTop);
        mAttackLabel.attachToComponent(&mAttackSlider, false);

        //Decay Slider
        mDecaySlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
        mDecaySlider.setTextBoxStyle(Slider::TextBoxBelow, true, 40, 20);
        mDecaySlider.setRange(0.0f, 12.0f, 0.01f);
        mDecaySlider.setSkewFactor(0.6f, false);
        addAndMakeVisible(mDecaySlider);

        mDecayLabel.setFont(10.0f);
        mDecayLabel.setText("Decay", NotificationType::dontSendNotification);
        mDecayLabel.setJustificationType(Justification::centredTop);
        mDecayLabel.attachToComponent(&mDecaySlider, false);

        //Sustain Slider
        mSustainSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
        mSustainSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 40, 20);
        mSustainSlider.setRange(0.0f, 1.0f, 0.01f);
        addAndMakeVisible(mSustainSlider);

        mSustainLabel.setFont(10.0f);
        mSustainLabel.setText("Sustain", NotificationType::dontSendNotification);
        mSustainLabel.setJustificationType(Justification::centredTop);
        mSustainLabel.attachToComponent(&mSustainSlider, false);

        //Release Slider
        mReleaseSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
        mReleaseSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 40, 20);
        mReleaseSlider.setRange(0.0f, 20.0f, 0.01f);
        mReleaseSlider.setSkewFactor(0.5f, false);
        addAndMakeVisible(mReleaseSlider);

        mReleaseLabel.setFont(10.0f);
        mReleaseLabel.setText("Release", NotificationType::dontSendNotification);
        mReleaseLabel.setJustificationType(Justification::centredTop);
        mReleaseLabel.attachToComponent(&mReleaseSlider, false);
    }

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



    if (mShouldBePainting)
    {
        Path p;
        mAudioPoints.clear();

        auto waveForm = processor.getWaveForm();
        auto ratio = waveForm.getNumSamples() / getWidth();
        auto buffer = waveForm.getReadPointer(0);

        //scale x
        for (int sample = 0; sample < waveForm.getNumSamples(); sample += ratio)
        {
            mAudioPoints.push_back(buffer[sample]);
        }

        // Start the path
        p.startNewSubPath(0, getHeight() / 2);

        // scale y
        for (int sample = 0; sample < mAudioPoints.size(); ++sample)
        {
            auto point = jmap<float>(mAudioPoints[sample], -1.0f, 1.0f, 200.0f, 0.0f);

            // Draw the line
            p.lineTo(sample, point);
        }

        g.strokePath(p, PathStrokeType(2));

        mShouldBePainting = false;
    }


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

bool SammyAudioProcessorEditor::isInterestedInFileDrag(const StringArray& files)
{
    for (auto file : files)
    {
        // Make sure all audio suported audio formats have been added.
        if (file.endsWith(".wav") || file.endsWith(".mp3") || file.endsWith(".aif"))
        {
            return true;
        }
    }
    // Give user some indication that their file type is not suported and return the supported file types.

    return false;
}

void SammyAudioProcessorEditor::filesDropped(const StringArray& files, int x, int y)
{
    for (auto file : files)
    {
        if (isInterestedInFileDrag(file))
        {
            mShouldBePainting = true;
            processor.loadFile(file);
        }
    }
    repaint();

    // TBA: For each file dropped add it to an empty sampler. If there are not enough empty samplers give a popup and only add the first files in the array. 
}
