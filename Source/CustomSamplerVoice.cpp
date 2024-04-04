/*
  ==============================================================================

    CustomSamplerVoice.cpp
    Created: 27 Feb 2024 4:50:46pm
    Author:  matth

  ==============================================================================
*/

#include "CustomSamplerVoice.h"
#include "CustomSamplerSound.h"

CustomSamplerVoice::CustomSamplerVoice() {}
CustomSamplerVoice::~CustomSamplerVoice() {}

bool CustomSamplerVoice::canPlaySound(SynthesiserSound* sound)
{
    return dynamic_cast<const CustomSamplerSound*> (sound) != nullptr;
}

void CustomSamplerVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound* s, int /*currentPitchWheelPosition*/)
{
    if (auto* sound = dynamic_cast<const CustomSamplerSound*> (s))
    {
        pitchRatio = std::pow(2.0, (midiNoteNumber - sound->midiRootNote) / 12.0)
            * sound->sourceSampleRate / getSampleRate();

        float randomStart = random.nextFloat();
        DBG(randomStart);
        DBG(sound->startRand);

        sourceSamplePosition = sound->length / 100 * sound->startPos + sound->startRand * randomStart;
        lgain = velocity;
        rgain = velocity;

        adsr.setSampleRate(sound->sourceSampleRate);
        adsr.setParameters(sound->params);

        adsr.noteOn();
    }
    else
    {
        jassertfalse; // this object can only play SamplerSounds!
    }
}

void CustomSamplerVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        adsr.noteOff();
    }
    else
    {
        clearCurrentNote();
        adsr.reset();
    }
}

void CustomSamplerVoice::pitchWheelMoved(int /*newValue*/) {}
void CustomSamplerVoice::controllerMoved(int /*controllerNumber*/, int /*newValue*/) {}

//==============================================================================
void CustomSamplerVoice::renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (auto* playingSound = static_cast<CustomSamplerSound*> (getCurrentlyPlayingSound().get()))
    {
        auto& data = *playingSound->data;
        const float* const inL = data.getReadPointer(0);
        const float* const inR = data.getNumChannels() > 1 ? data.getReadPointer(1) : nullptr;

        float* outL = outputBuffer.getWritePointer(0, startSample);
        float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer(1, startSample) : nullptr;

        while (--numSamples >= 0)
        {
            auto pos = (int)sourceSamplePosition;
            auto alpha = (float)(sourceSamplePosition - pos);
            auto invAlpha = 1.0f - alpha;

            // just using a very simple linear interpolation here..
            float l = (inL[pos] * invAlpha + inL[pos + 1] * alpha);
            float r = (inR != nullptr) ? (inR[pos] * invAlpha + inR[pos + 1] * alpha)
                : l;

            auto envelopeValue = adsr.getNextSample();

            l *= lgain * envelopeValue;
            r *= rgain * envelopeValue;

            if (outR != nullptr)
            {
                *outL++ += l;
                *outR++ += r;
            }
            else
            {
                *outL++ += (l + r) * 0.5f;
            }

            sourceSamplePosition += pitchRatio;

            if (sourceSamplePosition > playingSound->length)
            {
                stopNote(0.0f, false);
                break;
            }
        }
    }
}



