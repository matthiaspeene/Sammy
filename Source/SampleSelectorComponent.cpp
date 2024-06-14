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
    for (int i = 0; i < buttonCount; ++i)
    {
        sampleButtons[i].setButtonText("Empty " + juce::String(i + 1));
        sampleButtons[i].onClick = [this, i] { sampleButtonClicked(i); };
        addAndMakeVisible(sampleButtons[i]);
    }

    clearButton.setButtonText("Remove Sample");
    clearButton.onClick = [this]() { clearSampler(); };
    addAndMakeVisible(clearButton);
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

    clearButton.setBounds(area.removeFromRight(120));

    auto buttonWidth = area.getWidth() / 8;

    for (int i = 0; i < buttonCount; ++i)
    {
        sampleButtons[i].setBounds(i * buttonWidth, 0, buttonWidth, 30);
    }
}

void SampleSelectorComponent::sampleLoaded(int index)
{
    sampleButtons[index].setButtonText(processor.getSampleName());
}

void SampleSelectorComponent::clearSampler()
{
    processor.removeCurrentSample();
    sampleButtons[processor.getSelectedSampleIndex()].setButtonText("Empty" + processor.getSelectedSampleIndex());
}

void SampleSelectorComponent::sampleButtonClicked(int index)
{
    if (onSampleButtonClicked)
    {
        onSampleButtonClicked(index);
    }
}