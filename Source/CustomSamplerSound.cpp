/*
  ==============================================================================

    CustomSamplerSound.cpp
    Created: 27 Feb 2024 5:22:20pm
    Author:  matth

  ==============================================================================
*/

#include "CustomSamplerSound.h"

CustomSamplerSound::CustomSamplerSound(const String& soundName,
    AudioFormatReader& source,
    const BigInteger& notes,
    int midiNoteForNormalPitch,
    double attackTimeSecs,
    double releaseTimeSecs,
    double maxSampleLengthSeconds)
    : name(soundName),
    sourceSampleRate(source.sampleRate),
    midiNotes(notes),
    midiRootNote(midiNoteForNormalPitch)
{
    if (sourceSampleRate > 0 && source.lengthInSamples > 0)
    {
        length = jmin((int)source.lengthInSamples,
            (int)(maxSampleLengthSeconds * sourceSampleRate));

        data.reset(new AudioBuffer<float>(jmin(2, (int)source.numChannels), length + 4));

        source.read(data.get(), 0, length + 4, 0, true, true);

        params.attack = static_cast<float> (attackTimeSecs);
        params.release = static_cast<float> (releaseTimeSecs);
    }
}

CustomSamplerSound::~CustomSamplerSound()
{
}

bool CustomSamplerSound::appliesToNote(int midiNoteNumber)
{
    return midiNotes[midiNoteNumber];
}

bool CustomSamplerSound::appliesToChannel(int /*midiChannel*/)
{
    return true;
}