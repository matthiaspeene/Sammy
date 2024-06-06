/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "CustomSamplerVoice.h"
#include "CustomSamplerSound.h"

//==============================================================================
SammyAudioProcessor::SammyAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    mAPVTS(*this, nullptr, "PARAMETERS", createParameters())

#endif
{
    mFormatManager.registerBasicFormats();
    mAPVTS.state.addListener(this);

    for (int i = 0; i < mNumVoices; i++)
    {
        mSampler.addVoice(new CustomSamplerVoice());
    }

    initializeSampleSettings(8);
}

SammyAudioProcessor::~SammyAudioProcessor()
{
    mFormatReaders.clear();
}

//==============================================================================
const juce::String SammyAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SammyAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool SammyAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool SammyAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double SammyAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SammyAudioProcessor::getNumPrograms()
{
    return 1;
}

int SammyAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SammyAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String SammyAudioProcessor::getProgramName(int index)
{
    return {};
}

void SammyAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void SammyAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    mSampler.setCurrentPlaybackSampleRate(sampleRate);
    for (int i = 0; i < mSampleSettings.size(); ++i)
    {
        updateAllParameters(i);
    }
}

void SammyAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SammyAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void SammyAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    if (mParametersShouldUpdate)
    {
        for (int i = 0; i < mSampleSettings.size(); ++i)
        {
            updateAllParameters(i);
        }
        mParametersShouldUpdate = false;
    }

    MidiMessage m;
    MidiBuffer::Iterator it{ midiMessages };
    int sample;

    while (it.getNextEvent(m, sample))
    {
        if (m.isNoteOn())
        {
            mIsNotePlaying = true;
            //pitchRatio = std::pow(2.0, ((m.getNoteNumber()) - midiNoteForNormalPitch + mSampleSettings[mSelectedSampleIndex].pitchOffset) / 12.0);
        }
        else if (m.isNoteOff())
        {
            mIsNotePlaying = false;
        }
    }

    mSampleCount = mIsNotePlaying ? mSampleCount + buffer.getNumSamples() : 0;
    mSampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool SammyAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* SammyAudioProcessor::createEditor()
{
    return new SammyAudioProcessorEditor(*this);
}

//==============================================================================
void SammyAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
}

void SammyAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
}

void SammyAudioProcessor::initializeSampleSettings(int numSamples)
{
    mSampleSettings.resize(numSamples);

    for (auto& settings : mSampleSettings)
    {
        settings.adsrParams = { 0.1f, 0.1f, 0.8f, 0.1f };
        settings.startPos = 0.0f;
        settings.startRandom = 0.0f;
        settings.pitchOffset = 0.0;
    }
}


void SammyAudioProcessor::updateAllParameters(int sampleIndex)
{
    updateADSR(sampleIndex);
    updateStartPos(sampleIndex);
    updateStartRandom(sampleIndex);
    updatePitch(sampleIndex);
}

void SammyAudioProcessor::loadFile()
{
    FileChooser chooser{ "Load File" };

    if (chooser.browseForFileToOpen())
    {
        auto file = chooser.getResult();
        std::unique_ptr<AudioFormatReader> reader(mFormatManager.createReaderFor(file));

        if (reader != nullptr && reader->numChannels <= getTotalNumOutputChannels())
        {
            BigInteger range;
            range.setRange(0, 128, true);

            mSampleSettings.push_back({});
            mFormatReaders.push_back(std::move(reader));
            int sampleIndex = mSampleSettings.size() - 1;

            mSampler.addSound(new CustomSamplerSound("Sample", *mFormatReaders.back(), range, 60, mSampleSettings[sampleIndex].adsrParams.attack, mSampleSettings[sampleIndex].adsrParams.release, 120.0));

            updateAllParameters(sampleIndex);
            selectSample(sampleIndex);
        }
        else
        {
            if (reader == nullptr)
                DBG("Error: Failed to load audio file.");
            else
                DBG("Error: Incompatible number of channels.");
        }
    }
}

bool SammyAudioProcessor::loadFile(const juce::String& path, int sampleIndex)
{
    if (mSampler.getSound(sampleIndex) != nullptr)
    {
        mSampler.removeSound(sampleIndex);
        DBG("Sound Removed");
    }

    auto file = File(path);

    if (!file.existsAsFile())
    {
        DBG("Error: File does not exist.");
        return false;
    }

    std::unique_ptr<AudioFormatReader> reader(mFormatManager.createReaderFor(file));

    if (reader->numChannels > getTotalNumOutputChannels())
    {
        DBG("Error: Failed to load audio file or incompatible number of channels.");
        return false;
    }

    auto sampleLength = static_cast<int>(reader->lengthInSamples);
    mSampleSettings[sampleIndex].audioBuffer.setSize(reader->numChannels, sampleLength);
    reader->read(&mSampleSettings[sampleIndex].audioBuffer, 0, sampleLength, 0, true, true);

    BigInteger range;
    range.setRange(0, 128, true);

    DBG(path.substring(path.lastIndexOf("\\") + 1));
    mSampler.addSound(new CustomSamplerSound(path.substring(path.lastIndexOf("\\") + 1), *reader, range, midiNoteForNormalPitch, mSampleSettings[sampleIndex].adsrParams.attack, mSampleSettings[sampleIndex].adsrParams.release, 120.0));
    return true;
}


void SammyAudioProcessor::updateADSR(int sampleIndex)
{
    auto& adsrParams = mSampleSettings[sampleIndex].adsrParams;
    adsrParams.attack = mAPVTS.getRawParameterValue("ATTACK")->load();
    adsrParams.decay = mAPVTS.getRawParameterValue("DECAY")->load();
    adsrParams.sustain = mAPVTS.getRawParameterValue("SUSTAIN")->load();
    adsrParams.release = mAPVTS.getRawParameterValue("RELEASE")->load();

    if (auto sound = dynamic_cast<CustomSamplerSound*>(mSampler.getSound(sampleIndex).get()))
    {
        sound->setEnvelopeParameters(adsrParams);
    }
}

void SammyAudioProcessor::updateStartPos(int sampleIndex)
{
    auto& startPos = mSampleSettings[sampleIndex].startPos;
    startPos = mAPVTS.getRawParameterValue("START")->load();

    if (auto sound = dynamic_cast<CustomSamplerSound*>(mSampler.getSound(sampleIndex).get()))
    {
        sound->setStartPos(startPos);
    }
}

void SammyAudioProcessor::updateStartRandom(int sampleIndex)
{
    auto& startRandom = mSampleSettings[sampleIndex].startRandom;
    startRandom = mAPVTS.getRawParameterValue("RANDOMS")->load();

    if (auto sound = dynamic_cast<CustomSamplerSound*>(mSampler.getSound(sampleIndex).get()))
    {
        sound->setStartRandom(startRandom);
    }
}

void SammyAudioProcessor::updatePitch(int sampleIndex)
{
    auto& pitchOffset = mSampleSettings[sampleIndex].pitchOffset;
    pitchOffset = mAPVTS.getRawParameterValue("PITCH OFFSET")->load();

    if (auto sound = dynamic_cast<CustomSamplerSound*>(mSampler.getSound(sampleIndex).get()))
    {
        sound->setPitchOffset(pitchOffset);
    }
}

float SammyAudioProcessor::getPitchRatio(int sampleIndex) const
{
    return std::pow(2.0, ((midiNoteForNormalPitch + mSampleSettings[sampleIndex].pitchOffset) / 12.0));
}

void SammyAudioProcessor::selectSample(int sampleIndex)
{
    mSelectedSampleIndex = sampleIndex;
    updateAllParameters(sampleIndex);
}

juce::AudioProcessorValueTreeState::ParameterLayout SammyAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> parameters;

    parameters.push_back(std::make_unique<AudioParameterFloat>("ATTACK", "Attack", 0.0f, 20.0f, 0.012f));
    parameters.push_back(std::make_unique<AudioParameterFloat>("DECAY", "Decay", 0.0f, 12.0f, 1.5f));
    parameters.push_back(std::make_unique<AudioParameterFloat>("SUSTAIN", "Sustain", 0.0f, 1.0f, 1.0f));
    parameters.push_back(std::make_unique<AudioParameterFloat>("RELEASE", "Release", 0.0f, 20.0f, 0.012f));
    parameters.push_back(std::make_unique<AudioParameterFloat>("START", "Start Position", 0.f, 100.f, 0.f));
    parameters.push_back(std::make_unique<AudioParameterFloat>("RANDOMS", "Start Position Randomization", 0.f, 100.f, 0.f));
    parameters.push_back(std::make_unique<AudioParameterFloat>("PITCH OFFSET", "Pitch Offset", -60.f, 60.f, 0.f));

    return { parameters.begin(), parameters.end() };
}

void SammyAudioProcessor::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    mParametersShouldUpdate = true;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SammyAudioProcessor();
}
