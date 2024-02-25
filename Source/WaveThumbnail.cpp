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
WaveThumbnail::WaveThumbnail(SammyAudioProcessor& p) : processor (p)
{

}

WaveThumbnail::~WaveThumbnail()
{
}

void WaveThumbnail::paint (juce::Graphics& g)
{
    g.fillAll(Colours::cadetblue.brighter());

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
