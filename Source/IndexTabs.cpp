/*
  ==============================================================================

    IndexTabs.cpp
    Created: 23 Apr 2024 9:48:16pm
    Author:  matth

  ==============================================================================
*/

#include <JuceHeader.h>
#include "IndexTabs.h"

//==============================================================================
IndexTabs::IndexTabs(SammyAudioProcessor& p)
    : processor(p)
{
    for (int i = 0; i < 12; i++)
    {
        mIndexButtons[i].setButtonText(std::to_string(i + 1));

        addAndMakeVisible(mIndexButtons[i]);

        int indexCopy = i;
        mIndexButtons[i].onClick = [this, indexCopy]() { processor.setIndex(indexCopy); };
    }

    mMatrixButton.setButtonText("Matrix (tba)");
    addAndMakeVisible(mMatrixButton);

}

IndexTabs::~IndexTabs()
{

}

void IndexTabs::paint (juce::Graphics& g)
{

}

void IndexTabs::resized()
{
    for (int i = 0; i < 12; i++)
    {
        mIndexButtons[i].setBounds(i * 54, 0, 54, getHeight());
    }
    mMatrixButton.setBounds(12 * 54, 0, getWidth() - 12 * 54, getHeight());
}

