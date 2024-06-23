#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class FilterComponent : public juce::Component
{
public:
    FilterComponent(SammyAudioProcessor& p, juce::Colour& bgColour, juce::Colour& midColour, juce::Colour& darkColour,
        juce::Colour& modColour, juce::Colour& modulatorColour);
    ~FilterComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:

    SammyAudioProcessor& processor;
    juce::Colour& bgColour;
    juce::Colour& midColour;
    juce::Colour& darkColour;
    juce::Colour& modColour;
    juce::Colour& modulatorColour;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterComponent)
};
