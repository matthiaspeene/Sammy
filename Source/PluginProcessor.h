/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class SammyAudioProcessor  : public juce::AudioProcessor,
                             public juce::ValueTree::Listener
{
public:
    //==============================================================================
    SammyAudioProcessor();
    ~SammyAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void loadFile();
    bool loadFile(const String& path);

    int getNumSamlerSounds() { return mSampler.getNumSounds(); }

    AudioBuffer<float>& getWaveForm() { return mWaveForm; }

    AudioProcessorValueTreeState& getAPVTS() { return mAPVTS; }

    void updateADSR();
    void updateStartPos();
    ADSR::Parameters& getADSRParams() { return mADSRParams; }
    float& getStartPos() { return mStartPos; }

    std::atomic<bool>& isNotePlaying() { return mIsNotePlaying; }
    std::atomic<int>& getSampleCount() { return  mSampleCount; }

    Colour& getBgColour() { return bgColour; }
    Colour& getMidColour() { return midColour; }
    Colour& getDarkColour() { return darkColour; }
    Colour& getModColour() { return modColour; }
    Colour& getModulatorColour() { return modulatorColour; }

private:
    Synthesiser mSampler;
    const int mNumVoices{16};
    AudioBuffer<float> mWaveForm;

    ADSR::Parameters mADSRParams;
    float mStartPos{ 0 };

    AudioFormatManager mFormatManager;
    AudioFormatReader* mFormatReader{ nullptr };

    AudioProcessorValueTreeState mAPVTS;
    AudioProcessorValueTreeState::ParameterLayout createParameters();

    void valueTreePropertyChanged(ValueTree& treeWhoseProperyhasChanged, const Identifier& propery);

    std::atomic<bool> mADSRShouldUpdate{ false };
    std::atomic<bool> mStartPosShouldUpdate{ false };
    std::atomic<bool> mIsNotePlaying{ false };
    std::atomic<int> mSampleCount{ 0 };

    Colour bgColour{ 245, 252, 255 };
    Colour midColour{ 192, 234, 255 };
    Colour darkColour{ 146, 206, 236 };
    Colour modColour{ 255, 158, 158 };
    Colour modulatorColour{ 195, 255, 177 };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SammyAudioProcessor)
};
