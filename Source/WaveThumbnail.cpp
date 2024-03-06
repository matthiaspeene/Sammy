/*
  ==============================================================================

    WaveThumbnail.cpp
    Created: 26 Feb 2024 12:22:44am
    Author:  matth

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveThumbnail.h"

//==============================================================================
WaveThumbnail::WaveThumbnail(SammyAudioProcessor& p)
    : bgColour(p.getBgColour()),
    midColour(p.getMidColour()),
    darkColour(p.getDarkColour()),
    modColour(p.getModColour()),
    modulatorColour(p.getModulatorColour()),
    processor(p)
{
    mStartPosSlider.setSliderStyle(Slider::SliderStyle::LinearBar);
    mStartPosSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    mStartPosSlider.setRange(0.f, 1.f, 0.001f);
    mStartPosSlider.setColour(Slider::ColourIds::trackColourId, midColour);
    mStartPosSlider.setAlpha(0.5f);
    addAndMakeVisible(mStartPosSlider);

    mStartPosAttachment = std::make_unique <AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "START", mStartPosSlider);
    mStartPosSlider.setValue(0.);
}

WaveThumbnail::~WaveThumbnail()
{

}

void WaveThumbnail::paint (juce::Graphics& g)
{
    g.fillAll(bgColour);

    if (mShouldBePainting && processor.getWaveForm().getNumSamples() > 0)
    {
        // TBA: Though the wave needs to be drawn every frame. the waveform dous not need to be updated every frame. Only update when file uploaded...
        mStartPosSlider.setEnabled(true);
        mStartPosSlider.setAlpha(0.5f);

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
            auto point = jmap<float>(mAudioPoints[sample], -1.0f, 1.0f, getHeight(), 0.0f);

            p.lineTo(sample, point);
        }

        g.setColour(midColour);
        g.fillPath(p);
        g.setColour(darkColour);
        g.strokePath(p, PathStrokeType(2));


        // TBA:: Get the samples played relative to playback speed. Also iterate trough all midi notes and have one play for each note. 
        auto startSliderPos = mStartPosSlider.getValue() / 100 * getWidth();
        
        auto playHeadPosition = jmap<int>(processor.getSampleCount(), 0, processor.getWaveForm().getNumSamples(), startSliderPos, getWidth());


        g.setColour(modulatorColour);
        g.drawLine(playHeadPosition, 0, playHeadPosition, getHeight(), 2.0f);

        g.setColour(modColour);
        g.drawLine(startSliderPos, 0, startSliderPos, getHeight(), 2.0f);

        g.fillRect(startSliderPos, 0.f, 100.f, 30.f);

    }
    else if (mShouldDisplayError)
    {
        mStartPosSlider.setAlpha(0.0f);
        mStartPosSlider.setEnabled(false);
        g.setColour(modColour);
        g.setFont(Font(48.0f));
        g.drawText("Failed to load file", getLocalBounds(), Justification::centred);
    }
    else
    {
        mStartPosSlider.setAlpha(0.0f);
        mStartPosSlider.setEnabled(false);
        g.setColour(darkColour);
        g.setFont(Font(48.0f));
        g.drawText("Load or drag in audio", getLocalBounds(), Justification::centred);
    }
}

void WaveThumbnail::resized()
{
    mStartPosSlider.setBoundsRelative(0.f, 0.f, 1.f, 1.f);
}

bool WaveThumbnail::isInterestedInFileDrag(const StringArray& files)
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

void WaveThumbnail::filesDropped(const StringArray& files, int x, int y)
{
    for (auto file : files)
    {
        if (isInterestedInFileDrag(file))
        {
            mShouldBePainting = true;
            if (!processor.loadFile(file))
            {
                mShouldDisplayError = true;
                mShouldBePainting = false;
            }
        }
    }
    mStartPosSlider.valueChanged();
    processor.updateStartPos();
    repaint();
}

//Slider Value Change
/*
void WaveThumbnail::sliderValueChange(Slider* slider)
{
    if (slider == &mStartPosSlider)
    {
        processor.getStartPos() = mStartPosSlider.getValue();

        DBG("Slider = " + std::to_string(mStartPosSlider.getValue()));
    }
    processor.updateStartPos();
}
*/

void WaveThumbnail::setColours(Colour& bg, Colour& mid, Colour& dark, Colour& mod, Colour& modulator)
{
    bgColour = bg;
    midColour = mid;
    darkColour = dark;
    modColour = mod;
    modulatorColour = modulator;
}
