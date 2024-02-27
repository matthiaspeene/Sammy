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

    updateADSR();
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

    if (mShouldUpdate)
    {
        updateADSR();
    }

    MidiMessage m;

    MidiBuffer::Iterator it{ midiMessages };
    int sample;

    while (it.getNextEvent (m, sample))
    {
        if (m.isNoteOn())
        {
            mIsNotePlaying = true;
            // Note is on but what note and how many? Check if I can get data on this and add visualization for each note.



        }
        else if (m.isNoteOff())
        {
            mIsNotePlaying = false;
            // Stop playhead
        }
    }

    mSampleCount = mIsNotePlaying ? mSampleCount += buffer.getNumSamples() : 0;


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

juce::AudioBuffer<float> downmixToStereo(const juce::AudioBuffer<float>& inputBuffer) // This idea has not yet been implimented.
{
    // Ensure inputBuffer has more than 2 channels
    jassert(inputBuffer.getNumChannels() > 2);

    int numSamples = inputBuffer.getNumSamples();
    juce::AudioBuffer<float> stereoBuffer(2, numSamples); // Create stereo buffer

    // Downmix each sample by averaging all channels
    for (int sample = 0; sample < numSamples; ++sample)
    {
        for (int channel = 0; channel < inputBuffer.getNumChannels(); ++channel)
        {
            // Add current channel's sample to corresponding stereo channel
            stereoBuffer.getWritePointer(0)[sample] += inputBuffer.getReadPointer(channel)[sample];
            stereoBuffer.getWritePointer(1)[sample] += inputBuffer.getReadPointer(channel)[sample];
        }
    }

    // Normalize stereo mix by dividing by the number of channels
    stereoBuffer.applyGain(1.0f / inputBuffer.getNumChannels());

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

            //TBA: Replace Sampler Sound with my own Sampler with configurable start and loop functionality. 
            mSampler.addSound(new CustomSamplerSound("Sample", *reader, range, 60, 0.1, 0.1, 120.0));

            updateADSR(); // TBA: check why this is here.
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
    mSampler.clearSounds();

    auto file = File(path);

    if (!file.existsAsFile())
    {
        DBG("Error: File does not exist.");
        return false;
    }

    std::unique_ptr<AudioFormatReader> reader(mFormatManager.createReaderFor(file));

    if (reader->numChannels > getTotalNumOutputChannels())
    {
        // TBA: Add a conversion from files with a higher output to stereo.

        DBG("Error: Failed to load audio file or incompatible number of channels.");
        return false;
    }

    auto sampleLength = static_cast<int>(reader->lengthInSamples);
    mWaveForm.setSize(1, sampleLength);
    reader->read(&mWaveForm, 0, sampleLength, 0, true, false);

    // Create a SamplerSound and add it to the sampler
    BigInteger range;
    range.setRange(0, 128, true);
    mSampler.addSound(new CustomSamplerSound("Sample", *reader, range, 60, 0.1, 0.1, 120.0));

    return true;
}


void SammyAudioProcessor::updateADSR()
{
    mADSRParams.attack = mAPVTS.getRawParameterValue("ATTACK")->load();
    mADSRParams.decay = mAPVTS.getRawParameterValue("DECAY")->load();
    mADSRParams.sustain = mAPVTS.getRawParameterValue("SUSTAIN")->load();
    mADSRParams.release = mAPVTS.getRawParameterValue("RELEASE")->load();

    for (int i = 0; i < mSampler.getNumSounds(); ++i)
    {
        if (auto sound = dynamic_cast<CustomSamplerSound*>(mSampler.getSound(i).get())) // This checks if the SynthesiserClass that were getting has a sampler sound and not a SynthesiserSound becouse synth has no adsr within.
        {
            sound->setEnvelopeParameters(mADSRParams);
        }
    }
}

AudioProcessorValueTreeState::ParameterLayout SammyAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> parameters;

    // To Do: Check out the AudioParameterFloat and see if the other constructor will be aplicable
    parameters.push_back(std::make_unique<AudioParameterFloat>("ATTACK", "Attack", 0.0f, 20.0f, 0.012f));
    parameters.push_back(std::make_unique<AudioParameterFloat>("DECAY", "Decay", 0.0f, 12.0f, 1.5f));
    parameters.push_back(std::make_unique<AudioParameterFloat>("SUSTAIN", "Sustain", 0.0f, 1.0f, 1.0f));
    parameters.push_back(std::make_unique<AudioParameterFloat>("RELEASE", "Release", 0.0f, 20.0f, 0.012f));

    return { parameters.begin(), parameters.end() };
}

void SammyAudioProcessor::valueTreePropertyChanged(ValueTree& treeWhoseProperyhasChanged, const Identifier& propery)
{
    mShouldUpdate = true;
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SammyAudioProcessor();
}
