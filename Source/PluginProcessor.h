/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include "CustomSamplerSound.h"

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

    void initializeSampleSettings();

    bool loadFile(const juce::String& path);

    int getNumSamplerSounds() const { return mNumVoices; }

    juce::AudioProcessorValueTreeState& getAPVTS() { return mAPVTS; }

    void updateADSR();
    void updateStartPos();
    void updateStartRandom();
    void updatePitch();
    void updateActiveMidiNotes(juce::BigInteger midiNotes);
    void updateRootNote(int rootNote);

    juce::ADSR::Parameters& getADSRParams() { return mSampleSettings[mSelectedSampleIndex].adsrParams; }
    float& getStartPos() { return mSampleSettings[mSelectedSampleIndex].startPos; }
    float& getStartRandom() { return mSampleSettings[mSelectedSampleIndex].startRandom; }

    juce::AudioBuffer<float>* getWaveForm() {
        if (auto sound = dynamic_cast<CustomSamplerSound*>(mSampleSettings[mSelectedSampleIndex].synth->getSound(0).get()))
        {
            return sound->getAudioData();
        }
        else
        {
            return nullptr;
        }
    }

    double getPlayHeadPos() {
        if (auto sound = dynamic_cast<CustomSamplerSound*>(mSampleSettings[mSelectedSampleIndex].synth->getSound(0).get()))
        {
            return sound->getPlayHeadPosition();
        }
        else {
            return 0.0;
        }
    }

    juce::BigInteger getMidiNotes()
    {
        if (auto sound = dynamic_cast<CustomSamplerSound*>(mSampleSettings[mSelectedSampleIndex].synth->getSound(0).get()))
        {
            return sound->getMidiRange();
        }
        else
        {
            juce::BigInteger range;
            range.setRange(0, 121, true);
            return range;
        }
    }

    int getMidiRootNote()
    {
        if (auto sound = dynamic_cast<CustomSamplerSound*>(mSampleSettings[mSelectedSampleIndex].synth->getSound(0).get()))
        {
            return sound->getMidiRootNote();
        }
        else
        {
            return 60;
        }
    }


    juce::String getSampleName() { return mSampleSettings[mSelectedSampleIndex].name; }


    std::atomic<bool>& isNotePlaying() { return mIsNotePlaying; }
    std::atomic<int>& getSampleCount() { return  mSampleCount; }

    juce::Colour& getBgColour() { return bgColour; }
    juce::Colour& getMidColour() { return midColour; }
    juce::Colour& getDarkColour() { return darkColour; }
    juce::Colour& getModColour() { return modColour; }
    juce::Colour& getModulatorColour() { return modulatorColour; }

    void selectSample(int sampleIndex);
    void removeCurrentSample();
    int getSelectedSampleIndex() const { return mSelectedSampleIndex; }

private:
    struct SampleSettings
    {
        juce::String name{ "Empty" };
        juce::ADSR::Parameters adsrParams;
        float startPos{ 0.f };
        float startRandom{ 0.f };
        double pitchOffset{ 0.f };

        std::unique_ptr<juce::Synthesiser> synth;

        SampleSettings()
            : synth(std::make_unique<juce::Synthesiser>())
        {
        }

        // Disable copying
        SampleSettings(const SampleSettings&) = delete;
        SampleSettings& operator=(const SampleSettings&) = delete;

        // Enable moving
        SampleSettings(SampleSettings&& other) noexcept
            : name(std::move(other.name)),
            adsrParams(std::move(other.adsrParams)),
            startPos(other.startPos),
            startRandom(other.startRandom),
            pitchOffset(other.pitchOffset),
            synth(std::move(other.synth))
        {
        }

        SampleSettings& operator=(SampleSettings&& other) noexcept
        {
            if (this != &other)
            {
                name = std::move(other.name);
                adsrParams = std::move(other.adsrParams);
                startPos = other.startPos;
                startRandom = other.startRandom;
                pitchOffset = other.pitchOffset;
                synth = std::move(other.synth);
            }
            return *this;
        }

        ~SampleSettings() = default;
    };


    const int mNumSamplers{ 8 };
    const int mNumVoices{ 4 };

    std::vector<SampleSettings> mSampleSettings;
    std::vector<std::unique_ptr<juce::AudioFormatReader>> mFormatReaders;

    int midiNoteForNormalPitch{ 60 };

    juce::AudioFormatManager mFormatManager;

    juce::AudioProcessorValueTreeState mAPVTS;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters(int maxSampleCount);

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

    std::atomic<bool> mParametersShouldUpdate{ false };
    std::atomic<bool> mIsNotePlaying{ false };
    std::atomic<int> mSampleCount{ 0 };

    juce::Colour bgColour{ 242, 242, 242 };
    juce::Colour midColour{ 133, 56, 166 };
    juce::Colour darkColour{ 71, 4, 89 };
    juce::Colour modColour{ 242, 202, 87 };
    juce::Colour modulatorColour{ 115, 134, 191 };

    void updateAllParameters(int sampleIndex);

    int mSelectedSampleIndex{ 0 };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SammyAudioProcessor)
};
