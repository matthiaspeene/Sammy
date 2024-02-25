/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SammyAudioProcessor::SammyAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    mFormatManager.registerBasicFormats();


    for (int i = 0; i < mNumVoices; i++)
    {
        mSampler.addVoice(new SamplerVoice());
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
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SammyAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SammyAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SammyAudioProcessor::getProgramName (int index)
{
    return {};
}

void SammyAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SammyAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mSampler.setCurrentPlaybackSampleRate(sampleRate);
}

void SammyAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SammyAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SammyAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    mSampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool SammyAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SammyAudioProcessor::createEditor()
{
    return new SammyAudioProcessorEditor (*this);
}

//==============================================================================
void SammyAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SammyAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void SammyAudioProcessor::loadFile()
{
    mSampler.clearSounds();

    FileChooser chooser{ "Load File" };

    if (chooser.browseForFileToOpen())
    {
        auto file = chooser.getResult();
        mFormatReader = mFormatManager.createReaderFor(file);
    }
    BigInteger range;
    range.setRange(0, 128, true);

    mSampler.addSound(new SamplerSound("Sample", *mFormatReader, range, 60, 0.1, 0.1, 120.0));
}

void SammyAudioProcessor::loadFile(const String& path)
{
    mSampler.clearSounds();

    auto file = File(path);

    mFormatReader = mFormatManager.createReaderFor(file);

    // Gets the waveform
    auto sampleLenght = static_cast<int>(mFormatReader->lengthInSamples);

    mWaveForm.setSize(1, sampleLenght);
    mFormatReader->read(&mWaveForm, 0, sampleLenght, 0, true, false);

    auto buffer = mWaveForm.getReadPointer(0);

    //Reads out sample buffer(waveform)
    /* 
    for (int sample = 0; sample < mWaveForm.getNumSamples(); ++sample)
    {
        DBG(buffer[sample]);
    }
    */ 

    BigInteger range;
    range.setRange(0, 128, true);

    mSampler.addSound(new SamplerSound("Sample", *mFormatReader, range, 60, 0.1, 0.1, 120.0));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SammyAudioProcessor();
}
