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

    mSampleSettings.resize(mNumSamplers);
    initializeSampleSettings();
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
    for (auto& settings : mSampleSettings)
    {
        settings.synth->setCurrentPlaybackSampleRate(sampleRate);
    }

    updateAllParameters(mSelectedSampleIndex); // Do we even need this one?
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

    // Clear unused output channels
    buffer.clear(getTotalNumInputChannels(), buffer.getNumSamples());

    if (mParametersShouldUpdate)
    {
        updateAllParameters(mSelectedSampleIndex);
        mParametersShouldUpdate = false;
    }

    // Process MIDI messages and audio
    for (auto& settings : mSampleSettings)
    {
        if (settings.synth.get()->getSound(0) != nullptr)
        {
            settings.synth->renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
        }
    }
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

void SammyAudioProcessor::initializeSampleSettings()
{
    for (auto& settings : mSampleSettings)
    {
        settings.synth->clearVoices();
        for (int i = 0; i < mNumVoices; ++i)
        {
            settings.synth->addVoice(new CustomSamplerVoice());
        }
        settings.synth->clearSounds();
    }
}

void SammyAudioProcessor::updateAllParameters(int sampleIndex)
{
    updateADSR(sampleIndex);
    updateStartPos(sampleIndex);
    updateStartRandom(sampleIndex);
    updatePitch(sampleIndex);
}

bool SammyAudioProcessor::loadFile(const juce::String& path, int sampleIndex)
{
    auto& sampleSettings = mSampleSettings[sampleIndex];
    sampleSettings.synth->clearSounds();

    auto file = juce::File(path);

    if (!file.existsAsFile())
    {
        DBG("Error: File does not exist.");
        return false;
    }

    std::unique_ptr<juce::AudioFormatReader> reader(mFormatManager.createReaderFor(file));

    if (reader->numChannels > getTotalNumOutputChannels())
    {
        DBG("Error: Failed to load audio file or incompatible number of channels.");
        return false;
    }

    auto sampleLength = static_cast<int>(reader->lengthInSamples);
    sampleSettings.audioBuffer.setSize(reader->numChannels, sampleLength);
    reader->read(&sampleSettings.audioBuffer, 0, sampleLength, 0, true, true);

    juce::BigInteger range;
    range.setRange(0, 128, true);

    sampleSettings.synth->addSound(new CustomSamplerSound(path.substring(path.lastIndexOf("\\") + 1),
        *reader,
        range,
        60,
        sampleSettings.adsrParams.attack,
        sampleSettings.adsrParams.release,
        120.0));

    DBG(path.substring(path.lastIndexOf("\\") + 1));

    return true;
}

void SammyAudioProcessor::updateADSR(int sampleIndex)
{
    auto& adsrParams = mSampleSettings[sampleIndex].adsrParams;

    if (auto sound = dynamic_cast<CustomSamplerSound*>(mSampleSettings[sampleIndex].synth->getSound(0).get()))
    {
        sound->setEnvelopeParameters(adsrParams);
    }
}

void SammyAudioProcessor::updateStartPos(int sampleIndex)
{
    auto& startPos = mSampleSettings[sampleIndex].startPos;

    if (auto sound = dynamic_cast<CustomSamplerSound*>(mSampleSettings[sampleIndex].synth->getSound(0).get()))
    {
        sound->setStartPos(startPos);
    }
}

void SammyAudioProcessor::updateStartRandom(int sampleIndex)
{
    auto& startRandom = mSampleSettings[sampleIndex].startRandom;

    if (auto sound = dynamic_cast<CustomSamplerSound*>(mSampleSettings[sampleIndex].synth->getSound(0).get()))
    {
        sound->setStartRandom(startRandom);
    }
}

void SammyAudioProcessor::updatePitch(int sampleIndex)
{
    auto& pitchOffset = mSampleSettings[sampleIndex].pitchOffset;

    if (auto sound = dynamic_cast<CustomSamplerSound*>(mSampleSettings[sampleIndex].synth->getSound(0).get()))
    {
        sound->setPitchOffset(pitchOffset);
    }
}

float SammyAudioProcessor::getPitchRatio(int sampleIndex) const
{
    return std::pow(2.0, ((/*midiNoteForNormalPitch * we should get this from the synth*/ 60 + mSampleSettings[sampleIndex].pitchOffset) / 12.0));
}

void SammyAudioProcessor::selectSample(int sampleIndex)
{
    mSelectedSampleIndex = sampleIndex;
    updateAllParameters(mSelectedSampleIndex);
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
