/*
  ==============================================================================

    CustomSamplerVoice.h
    Created: 27 Feb 2024 4:50:46pm
    Author:  matth

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class   CustomSamplerVoice : public SynthesiserVoice
{
public:
    //==============================================================================
    /** Creates a SamplerVoice. */
    CustomSamplerVoice();

    /** Destructor. */
    ~CustomSamplerVoice() override;

    //==============================================================================
    bool canPlaySound(SynthesiserSound*) override;

    void startNote(int midiNoteNumber, float velocity, SynthesiserSound*, int pitchWheel) override;
    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newValue) override;
    void controllerMoved(int controllerNumber, int newValue) override;

    void renderNextBlock(AudioBuffer<float>&, int startSample, int numSamples) override;
    using SynthesiserVoice::renderNextBlock;

private:
    //==============================================================================
    double pitchRatio = 0;
    double sourceSamplePosition = 0;
    float lgain = 0, rgain = 0;

    ADSR adsr;

    JUCE_LEAK_DETECTOR(SamplerVoice)
};