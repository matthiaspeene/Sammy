/*
  ==============================================================================

    SimpleCompression.cpp
    Created: 18 Jun 2024 6:46:39pm
    Author:  matth

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SimpleCompression.h"

SimpleCompression::SimpleCompression(SammyAudioProcessor& p, juce::Colour& bgCol, juce::Colour& midCol, juce::Colour& darkCol, juce::Colour& modCol, juce::Colour& modulatorCol)
    : processor(p), bgColour(bgCol), midColour(midCol), darkColour(darkCol), modColour(modCol), modulatorColour(modulatorCol)
{
}

SimpleCompression::~SimpleCompression()
{

}

void SimpleCompression::paint(juce::Graphics& g)
{
    g.setColour(bgColour);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 20.0f);

    g.setColour(darkColour);
    g.drawText("TBA: SimpleCompression", getLocalBounds(),
        juce::Justification::centred, true);   // draw some placeholder text
}

void SimpleCompression::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
