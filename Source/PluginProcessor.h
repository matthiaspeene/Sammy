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
class SammyAudioProcessor : public juce::AudioProcessor,
    public juce::ValueTree::Listener
{
public:
    //==============================================================================
    SammyAudioProcessor();
    ~SammyAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void loadFile();
    bool loadFile(const juce::String& path);

    int getNumSamplerSounds() const { return mSampler.getNumSounds(); }

    juce::AudioBuffer<float>& getWaveForm() { return mWaveForm; }

    juce::AudioProcessorValueTreeState& getAPVTS() { return mAPVTS; }

    void updateADSR();
    void updateStartPos();
    void updateStartRandom();
    void updatePitch();

    float getPitchRatio();

    juce::ADSR::Parameters& getADSRParams() { return mADSRParams; }
    float getStartPos() const { return mStartPos; }

    std::atomic<bool>& isNotePlaying() { return mIsNotePlaying; }
    std::atomic<int>& getSampleCount() { return  mSampleCount; }

    juce::Colour& getBgColour() { return bgColour; }
    juce::Colour& getMidColour() { return midColour; }
    juce::Colour& getDarkColour() { return darkColour; }
    juce::Colour& getModColour() { return modColour; }
    juce::Colour& getModulatorColour() { return modulatorColour; }

private:
    juce::Synthesiser mSampler;
    const int mNumVoices{ 16 };
    juce::AudioBuffer<float> mWaveForm;

    float pitchRatio{ 1.0f };
    int midiNoteForNormalPitch{ 60 };

    juce::ADSR::Parameters mADSRParams;
    float mStartPos{ 0.f };
    float mStartRandom{ 0.f };
    double mPitchOffset{ 0.0 };

    juce::AudioFormatManager mFormatManager;
    juce::AudioFormatReader* mFormatReader{ nullptr };

    juce::AudioProcessorValueTreeState mAPVTS;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

    int samplerIndex{ 0 };

    std::atomic<bool> mParametersShouldUpdate{ false };
    std::atomic<bool> mIsNotePlaying{ false };
    std::atomic<int> mSampleCount{ 0 };

    juce::Colour bgColour{ 245, 252, 255 };
    juce::Colour midColour{ 192, 234, 255 };
    juce::Colour darkColour{ 146, 206, 236 };
    juce::Colour modColour{ 255, 158, 158 };
    juce::Colour modulatorColour{ 195, 255, 177 };

    void updateAllParameters();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SammyAudioProcessor)
};
