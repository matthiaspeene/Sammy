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
}

SammyAudioProcessor::~SammyAudioProcessor()
{
    mFormatReader = nullptr;
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
    updateAllParameters();
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
        updateAllParameters();
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
            pitchRatio = std::pow(2.0, ((m.getNoteNumber()) - midiNoteForNormalPitch + mPitchOffset) / 12.0);
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

void SammyAudioProcessor::updateAllParameters()
{
    updateADSR();
    updateStartPos();
    updateStartRandom();
    updatePitch();
}

juce::AudioBuffer<float> downmixToStereo(const juce::AudioBuffer<float>& inputBuffer)
{
    jassert(inputBuffer.getNumChannels() > 2);

    int numSamples = inputBuffer.getNumSamples();
    juce::AudioBuffer<float> stereoBuffer(2, numSamples);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float leftSample = 0.0f;
        float rightSample = 0.0f;

        for (int channel = 0; channel < inputBuffer.getNumChannels(); ++channel)
        {
            leftSample += inputBuffer.getReadPointer(channel)[sample];
            rightSample += inputBuffer.getReadPointer(channel)[sample];
        }

        stereoBuffer.getWritePointer(0)[sample] = leftSample / inputBuffer.getNumChannels();
        stereoBuffer.getWritePointer(1)[sample] = rightSample / inputBuffer.getNumChannels();
    }

    return stereoBuffer;
}

void SammyAudioProcessor::loadFile()
{
    mSampler.clearSounds();

    FileChooser chooser{ "Load File" };

    if (chooser.browseForFileToOpen())
    {
        auto file = chooser.getResult();
        std::unique_ptr<AudioFormatReader> reader(mFormatManager.createReaderFor(file));

        if (reader != nullptr && reader->numChannels <= getTotalNumOutputChannels())
        {
            BigInteger range;
            range.setRange(0, 128, true);

            mSampler.addSound(new CustomSamplerSound("Sample", *reader, range, 60, mADSRParams.attack, mADSRParams.release, 120.0));

            updateAllParameters();
        }
        else
        {
            if (reader == nullptr)
                DBG("Error: Failed to load audio file.");
            else
                DBG("Error: Incompatible number of channels.");

            reader.reset();
        }
    }
}

bool SammyAudioProcessor::loadFile(const String& path)
{
    if (mSampler.getSound(samplerIndex) != nullptr)
    {
        mSampler.removeSound(samplerIndex);
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
    mWaveForm.setSize(1, sampleLength);
    reader->read(&mWaveForm, 0, sampleLength, 0, true, false);

    BigInteger range;
    range.setRange(0, 128, true);

    DBG(path.substring(path.lastIndexOf("\\") + 1));
    mSampler.addSound(new CustomSamplerSound(path.substring(path.lastIndexOf("\\") + 1), *reader, range, midiNoteForNormalPitch, mADSRParams.attack, mADSRParams.release, 120.0));
    return true;
}

void SammyAudioProcessor::updateADSR()
{
    mADSRParams.attack = mAPVTS.getRawParameterValue("ATTACK")->load();
    mADSRParams.decay = mAPVTS.getRawParameterValue("DECAY")->load();
    mADSRParams.sustain = mAPVTS.getRawParameterValue("SUSTAIN")->load();
    mADSRParams.release = mAPVTS.getRawParameterValue("RELEASE")->load();

    if (auto sound = dynamic_cast<CustomSamplerSound*>(mSampler.getSound(samplerIndex).get()))
    {
        sound->setEnvelopeParameters(mADSRParams);
    }
}

void SammyAudioProcessor::updateStartPos()
{
    mStartPos = mAPVTS.getRawParameterValue("START")->load();

    if (auto sound = dynamic_cast<CustomSamplerSound*>(mSampler.getSound(samplerIndex).get()))
    {
        sound->setStartPos(mStartPos);
    }
}

void SammyAudioProcessor::updateStartRandom()
{
    mStartRandom = mAPVTS.getRawParameterValue("RANDOMS")->load();

    if (auto sound = dynamic_cast<CustomSamplerSound*>(mSampler.getSound(samplerIndex).get()))
    {
        sound->setStartRandom(mStartRandom);
    }
}

void SammyAudioProcessor::updatePitch()
{
    mPitchOffset = mAPVTS.getRawParameterValue("PITCH OFFSET")->load();

    if (auto sound = dynamic_cast<CustomSamplerSound*>(mSampler.getSound(samplerIndex).get()))
    {
        sound->setPitchOffset(mPitchOffset);
    }
}

float SammyAudioProcessor::getPitchRatio()
{
    return pitchRatio;
}

AudioProcessorValueTreeState::ParameterLayout SammyAudioProcessor::createParameters()
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

void SammyAudioProcessor::valueTreePropertyChanged(ValueTree& treeWhoseProperyhasChanged, const Identifier& propery)
{
    mParametersShouldUpdate = true;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SammyAudioProcessor();
}
