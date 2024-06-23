/*
  ==============================================================================

    PitchControll.cpp
    Created: 18 Jun 2024 6:46:39pm
    Author:  matth

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PitchControll.h"

PitchControll::PitchControll(SammyAudioProcessor& p, juce::Colour& bgCol, juce::Colour& midCol, juce::Colour& darkCol, juce::Colour& modCol, juce::Colour& modulatorCol)
    : processor(p), bgColour(bgCol), midColour(midCol), darkColour(darkCol), modColour(modCol), modulatorColour(modulatorCol)
{
}

PitchControll::~PitchControll()
{

}

void PitchControll::paint(juce::Graphics& g)
{
    g.setColour(bgColour);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 20.0f);

    g.setColour(darkColour);
    g.drawText("TBA: PitchControll", getLocalBounds(),
        juce::Justification::centred, true);   // draw some placeholder text
}

void PitchControll::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
