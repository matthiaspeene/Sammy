/*
  ==============================================================================

    SampleSelectorComponent.h
    Created: 26 Feb 2024 1:17:36pm
    Author:  matth

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class SampleSelectorComponent : public juce::Component
{
public:
    SampleSelectorComponent(SammyAudioProcessor& p);
    ~SampleSelectorComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    std::function<void(int)> onSampleButtonClicked;

private:
    juce::TextButton sampleButtons[8];
    SammyAudioProcessor& processor;

    void sampleButtonClicked(int index);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleSelectorComponent)
};
