/*
  ==============================================================================

    CustomSamplerSound.h
    Created: 27 Feb 2024 5:22:20pm
    Author:  matth

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class CustomSamplerSound : public SynthesiserSound
{
public:
    //==============================================================================
    /** Creates a sampled sound from an audio reader.

        This will attempt to load the audio from the source into memory and store
        it in this object.

        @param name         a name for the sample
        @param source       the audio to load. This object can be safely deleted by the
                            caller after this constructor returns
        @param midiNotes    the set of midi keys that this sound should be played on. This
                            is used by the SynthesiserSound::appliesToNote() method
        @param midiNoteForNormalPitch   the midi note at which the sample should be played
                                        with its natural rate. All other notes will be pitched
                                        up or down relative to this one
        @param attackTimeSecs   the attack (fade-in) time, in seconds
        @param releaseTimeSecs  the decay (fade-out) time, in seconds
        @param maxSampleLengthSeconds   a maximum length of audio to read from the audio
                                        source, in seconds
    */
    CustomSamplerSound(const String & name,
                  AudioFormatReader & source,
                  const BigInteger & midiNotes,
                  int midiNoteForNormalPitch,
                  double attackTimeSecs,
                  double releaseTimeSecs,
                  double maxSampleLengthSeconds);

    /** Destructor. */
    ~CustomSamplerSound() override;

    //==============================================================================
    /** Returns the sample's name */
    const String& getName() const noexcept { return name; }

    /** Returns the audio sample data.
        This could return nullptr if there was a problem loading the data.
    */
    AudioBuffer<float>* getAudioData() const noexcept { return data.get(); }

    //==============================================================================
    /** Changes the parameters of the ADSR envelope which will be applied to the sample. */
    void setEnvelopeParameters(ADSR::Parameters parametersToUse) { params = parametersToUse; }
    void setStartPos(int startPosistion) { startPos = startPosistion; }

    //==============================================================================
    bool appliesToNote(int midiNoteNumber) override;
    bool appliesToChannel(int midiChannel) override;

private:
    //==============================================================================
    friend class CustomSamplerVoice;

    String name;
    std::unique_ptr<AudioBuffer<float>> data;
    double sourceSampleRate;
    BigInteger midiNotes;
    int length = 0, midiRootNote = 0;
    int startPos;



    ADSR::Parameters params;

    JUCE_LEAK_DETECTOR(CustomSamplerSound)
};