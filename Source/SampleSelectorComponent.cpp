/*
  ==============================================================================

    SampleSelectorComponent.cpp
    Created: 26 Feb 2024 1:17:36pm
    Author:  matth

  ==============================================================================
*/

#include "SampleSelectorComponent.h"

SampleSelectorComponent::SampleSelectorComponent(SammyAudioProcessor& p) : processor(p)
{
    for (int i = 0; i < 8; ++i)
    {
        sampleButtons[i].setButtonText("Sample " + juce::String(i + 1));
        sampleButtons[i].onClick = [this, i] { sampleButtonClicked(i); };
        addAndMakeVisible(sampleButtons[i]);
    }
}

SampleSelectorComponent::~SampleSelectorComponent()
{
}

void SampleSelectorComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void SampleSelectorComponent::resized()
{
    auto area = getLocalBounds();
    auto buttonWidth = area.getWidth() / 8;

    for (int i = 0; i < 8; ++i)
    {
        sampleButtons[i].setBounds(i * buttonWidth, 0, buttonWidth, 30);
    }
}

void SampleSelectorComponent::sampleButtonClicked(int index)
{
    if (onSampleButtonClicked)
        onSampleButtonClicked(index);
}
