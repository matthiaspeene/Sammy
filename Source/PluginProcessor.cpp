#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "CustomSamplerVoice.h"

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
    // this might be optimized. All the below functions have repeating parts
    updateADSR();
    updateStartPos();
    updateStartRandom();
    updatePitch();
}

bool SammyAudioProcessor::loadFile(const juce::String& path)
{
    DBG("Loading file: " + path);
    auto& sampleSettings = mSampleSettings[mSelectedSampleIndex];
    sampleSettings.synth->clearSounds();

    auto file = juce::File(path);
    if (!file.existsAsFile())
    {
        DBG("Error: File does not exist.");
        return false;
    }

    auto reader = std::unique_ptr<juce::AudioFormatReader>(mFormatManager.createReaderFor(file));

    if (!reader) {
        DBG("Error: Failed to create audio format reader.");
        return false;
    }

    if (reader->numChannels > getTotalNumOutputChannels()) {
        DBG("Error: Incompatible number of channels.");
        return false;
    }


    juce::BigInteger range;
    range.setRange(0, 128, true);
    sampleSettings.name = path.substring(path.lastIndexOf("\\") + 1);

    sampleSettings.synth->addSound(new CustomSamplerSound(
        sampleSettings.name,
        *reader,
        range,
        60,
        sampleSettings.adsrParams.attack,
        sampleSettings.adsrParams.release,
        120.0));

    return true;
}



void SammyAudioProcessor::updateADSR()
{
    auto& adsrParams = mSampleSettings[mSelectedSampleIndex].adsrParams;

    if (auto sound = dynamic_cast<CustomSamplerSound*>(mSampleSettings[mSelectedSampleIndex].synth->getSound(0).get()))
    {
        sound->setEnvelopeParameters(adsrParams);
    }
}

void SammyAudioProcessor::updateStartPos()
{
    auto& startPos = mSampleSettings[mSelectedSampleIndex].startPos;

    if (auto sound = dynamic_cast<CustomSamplerSound*>(mSampleSettings[mSelectedSampleIndex].synth->getSound(0).get()))
    {
        sound->setStartPos(startPos);
    }
}

void SammyAudioProcessor::updateStartRandom()
{
    auto& startRandom = mSampleSettings[mSelectedSampleIndex].startRandom;

    if (auto sound = dynamic_cast<CustomSamplerSound*>(mSampleSettings[mSelectedSampleIndex].synth->getSound(0).get()))
    {
        sound->setStartRandom(startRandom);
    }
}

void SammyAudioProcessor::updatePitch()
{
    auto& pitchOffset = mSampleSettings[mSelectedSampleIndex].pitchOffset;

    if (auto sound = dynamic_cast<CustomSamplerSound*>(mSampleSettings[mSelectedSampleIndex].synth->getSound(0).get()))
    {
        sound->setPitchOffset(pitchOffset);
    }
}

void SammyAudioProcessor::selectSample(int sampleIndex)
{
    mSelectedSampleIndex = sampleIndex;
    updateAllParameters(mSelectedSampleIndex);
}

void SammyAudioProcessor::removeCurrentSample()
{
    auto& sampleSettings = mSampleSettings[mSelectedSampleIndex];
    sampleSettings.synth->clearSounds();
    sampleSettings.name = "Empty";
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
    parameters.push_back(std::make_unique<AudioParameterFloat>("PITCH OFFSET", "Pitch Offset", -36.f, 36.f, 0.f));

    return { parameters.begin(), parameters.end() };
}

void SammyAudioProcessor::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    mParametersShouldUpdate = true; // This makes it so that in the proccesor all the parameters will be updated. Can we optimize this? Only update the parameter that changed?
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SammyAudioProcessor();
}
