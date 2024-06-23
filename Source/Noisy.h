#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class Noisy : public juce::Component
{
public:
    Noisy(SammyAudioProcessor& p,juce::Colour& bgColour, juce::Colour& midColour, juce::Colour& darkColour,
        juce::Colour& modColour, juce::Colour& modulatorColour);
    ~Noisy() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:

    SammyAudioProcessor& processor;
    juce::Colour& bgColour;
    juce::Colour& midColour;
    juce::Colour& darkColour;
    juce::Colour& modColour;
    juce::Colour& modulatorColour;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Noisy)
};
