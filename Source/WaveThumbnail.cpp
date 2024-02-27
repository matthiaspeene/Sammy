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
    
}

WaveThumbnail::~WaveThumbnail()
{
}

void WaveThumbnail::paint (juce::Graphics& g)
{
    g.fillAll(bgColour);

    if (mShouldBePainting)
    {
        // TBA: Though the wave needs to be drawn every frame. the waveform dous not need to be updated every frame. Only update when file uploaded...

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

        g.setColour(darkColour);
        // scale y
        for (int sample = 0; sample < mAudioPoints.size(); ++sample)
        {
            auto point = jmap<float>(mAudioPoints[sample], -1.0f, 1.0f, getHeight(), 0.0f);

            // Draw the line
            p.lineTo(sample, point);
        }

        g.strokePath(p, PathStrokeType(2));

        // TBA:: Get the samples played relative to playback speed. Also iterate trough all midi notes and have one play for each note. 

        auto playHeadPosition = jmap<int>(processor.getSampleCount(), 0, processor.getWaveForm().getNumSamples(), 0, getWidth());

        g.setColour(modulatorColour);
        g.drawLine(playHeadPosition, 0, playHeadPosition, getHeight(), 2.0f);

    }
    else
    {
        // TBA Tell user to drag in sample. 
    }


}

void WaveThumbnail::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

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
            processor.loadFile(file);
        }
    }
    repaint();

    // TBA: For each file dropped add it to an empty sampler. If there are not enough empty samplers give a popup and only add the first files in the array. 
}

void WaveThumbnail::setColours(Colour& bg, Colour& mid, Colour& dark, Colour& mod, Colour& modulator)
{
    bgColour = bg;
    midColour = mid;
    darkColour = dark;
    modColour = mod;
    modulatorColour = modulator;
}
