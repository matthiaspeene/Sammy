/*
  ==============================================================================

    IndexTabs.h
    Created: 23 Apr 2024 9:48:16pm
    Author:  matth

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class IndexTabs  : public juce::Component
{
public:
    IndexTabs(SammyAudioProcessor& p);
    ~IndexTabs() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:

    TextButton mIndexButtons[12];

    TextButton mMatrixButton;

    SammyAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IndexTabs)
};
