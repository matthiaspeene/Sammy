/*
  ==============================================================================

    WaveThumbnail.cpp
    Created: 26 Feb 2024 12:22:44am
    Author:  matth

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveThumbnail.h"

//TBA:: Make this a viewport so the scrollbar can be used and the wave only needs to be drawn once.

//==============================================================================
WaveThumbnail::WaveThumbnail(SammyAudioProcessor& p)
    : bgColour(p.getBgColour()),
    midColour(p.getMidColour()),
    darkColour(p.getDarkColour()),
    modColour(p.getModColour()),
    modulatorColour(p.getModulatorColour()),
    waveform(p.getWaveForm()),
    processor(p)
{
    mStartPosSlider.setSliderStyle(Slider::SliderStyle::LinearBar);
    mStartPosSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    mStartPosSlider.setRange(0.f, 1.f, 0.001f);
    mStartPosSlider.setColour(Slider::ColourIds::trackColourId, midColour);
    mStartPosSlider.setAlpha(0.f);
    mStartPosSlider.setScrollWheelEnabled(false);
    addAndMakeVisible(mStartPosSlider);

    mStartPosAttachment = std::make_unique <AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "START", mStartPosSlider);
    mStartPosSlider.setValue(0.f);

    mRandomStartSlider.setSliderStyle(Slider::SliderStyle::LinearBar);
    mRandomStartSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    mRandomStartSlider.setRange(0.f, 1.f, 0.001f);
    mRandomStartSlider.setColour(Slider::ColourIds::trackColourId, midColour);
    mRandomStartSlider.setAlpha(0.f);
    mRandomStartSlider.setScrollWheelEnabled(false);
    addAndMakeVisible(mRandomStartSlider);

    mRandomStartAttachment = std::make_unique <AudioProcessorValueTreeState::SliderAttachment>(processor.getAPVTS(), "RANDOMS", mRandomStartSlider);
    mRandomStartSlider.setValue(0.f);

    mZoomSlider.setSliderStyle(Slider::SliderStyle::TwoValueHorizontal);
    mZoomSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    mZoomSlider.setRange(0.f, 1.f, 0.001f);
    mZoomSlider.setMaxValue(1.0f);
    mZoomSlider.setColour(Slider::ColourIds::trackColourId, midColour);
    addAndMakeVisible(mZoomSlider);

    mZoomSlider.onValueChange = [this]() {
        updateWaveForm();
        repaint();
        };
}

WaveThumbnail::~WaveThumbnail()
{

}

void WaveThumbnail::paint (juce::Graphics& g)
{
    g.fillAll(bgColour);

    if (mShouldBePainting && processor.getWaveForm().getNumSamples() > 0)
    {
        mStartPosSlider.setEnabled(true);
        mRandomStartSlider.setEnabled(true);

        auto startSliderPos = mStartPosSlider.getValue() / 100 / mZoomSlider.getMaxValue() * getWidth();
        //startSliderPos += mStartPosSlider.getValue() * -1 / 100 / (1 - mZoomSlider.getMinValue()) * getWidth();
        //startSliderPos /= 2;

        // TBA:: Get the samples played relative to playback speed. Also iterate trough all midi notes and have one play for each note. 
        auto playHeadPosition = jmap<int>(processor.getSampleCount(), 0, processor.getWaveForm().getNumSamples(), startSliderPos, getWidth());
        
        if (startSliderPos >= 0.f && startSliderPos <= getWidth())
        {
            g.setColour(modulatorColour);
            g.fillRect(0, 0, startSliderPos, getHeight());

            g.setColour(modulatorColour);
            g.drawLine(playHeadPosition, 0, playHeadPosition, getHeight(), 2.0f);

            
            g.setColour(modColour);
            g.drawLine(startSliderPos, 0, startSliderPos, getHeight(), 2.0f);

            auto mStartRandomLenght = (getWidth() - startSliderPos) * mRandomStartSlider.getValue() / 100;

            g.fillRect(startSliderPos, getHeight()-30.f, mStartRandomLenght, 30.f);
        }

        mZoomSlider.setEnabled(true);

        g.setColour(midColour);
        g.fillPath(wavePath[processor.getSampleIndex()]);
        g.setColour(darkColour);
        g.strokePath(wavePath[processor.getSampleIndex()], PathStrokeType(2));
    }
    else if (mShouldDisplayError)
    {
        mStartPosSlider.setEnabled(false);
        mRandomStartSlider.setEnabled(false);

        g.setColour(modColour);
        g.setFont(Font(48.0f));
        g.drawText("Failed to load file", getLocalBounds(), Justification::centred);
    }
    else
    {
        mStartPosSlider.setEnabled(false);
        mRandomStartSlider.setEnabled(false);

        g.setColour(darkColour);
        g.setFont(Font(48.0f));
        g.drawText("Load or drag in audio", getLocalBounds(), Justification::centred);
    }
}

void WaveThumbnail::resized()
{
    mStartPosSlider.setBoundsRelative(0.f, 0.f, 1.f, 0.8f);
    mRandomStartSlider.setBoundsRelative(0.f, 0.8f, 1.f, 0.2f);
    mZoomSlider.setBoundsRelative(0.f, 0.f, 1.f, 0.1f);
}

bool WaveThumbnail::isInterestedInFileDrag(const StringArray& files)
{
    for (auto file : files)
    {
        // TBA:: Check if all suported formats are here
        if (file.endsWith(".wav") || file.endsWith(".mp3") || file.endsWith(".aif"))
        {
            return true;
        }
    }
    // TBA: Add a popup that the file type is not supported and provide suported types...

    return false;
}

void WaveThumbnail::filesDropped(const StringArray& files, int x, int y)
{
    for (auto file : files)
    {
        if (isInterestedInFileDrag(file))
        {
            mShouldBePainting = true;
            if (!processor.loadFile(file))
            {
                mShouldDisplayError = true;
                mShouldBePainting = false;
            }
        }
    }
    mStartPosSlider.valueChanged();
    processor.updateStartPos();
    processor.updateStartRandom();
    updateWaveForm();
    repaint();
}

void WaveThumbnail::updateWaveForm()
{
    if (processor.getWaveForm().getNumSamples() > 0)
    {
        if (mZoomSlider.getMinValue() == mZoomSlider.getMaxValue())
        {
            mZoomSlider.setMaxValue(mZoomSlider.getMaxValue() + mZoomSlider.getInterval());

            if (mZoomSlider.getMinValue() == mZoomSlider.getMaxValue())
            {
                mZoomSlider.setMinValue(mZoomSlider.getMinValue() - mZoomSlider.getInterval());
            }
        }

        Path p;
        mAudioPoints.clear();

        const double leftSamplePos = mZoomSlider.getMinValue();
        const double rightSamplePos = mZoomSlider.getMaxValue();
        const int width = getWidth();

        const int numSamples = waveform.getNumSamples();

        const double ratio = numSamples / static_cast<double>(width) * (rightSamplePos - leftSamplePos);

        const float* buffer = waveform.getReadPointer(0);

        // Calculate the number of samples to average around each position
        const int samplesToAverage = ratio / 2;

        // Populate mAudioPoints with average values
        for (double sample = numSamples * leftSamplePos; sample < numSamples * rightSamplePos; sample += ratio) {
            if (sample >= 0 && sample < numSamples) {
                float sum = 0.0f;
                int count = 0;
                // Calculate the average value within a range around the current sample position
                for (int i = -samplesToAverage / 2; i <= samplesToAverage / 2; ++i) {
                    int index = static_cast<int>(sample) + i;
                    if (index >= 0 && index < numSamples) {
                        sum += buffer[index];
                        count++;
                    }
                }
                if (count > 0) {
                    float average = sum / count;
                    mAudioPoints.push_back(average);
                }
            }
        }

        // Find the maximum absolute value of the samples within the visible range
        float maxSample = 0.0f;
        for (int i = 0; i < mAudioPoints.size(); ++i) {
            float absSample = std::abs(mAudioPoints[i]);
            if (absSample > maxSample) {
                maxSample = absSample;
            }
        }

        // Start the path
        p.startNewSubPath(0, getHeight() / 2);

        // Scale y based on the maximum absolute value
        for (int sample = 0; sample < mAudioPoints.size(); ++sample)
        {
            auto point = jmap<float>(mAudioPoints[sample], -maxSample, maxSample, getHeight(), 0.0f);

            p.lineTo(sample, point);
        }
        wavePath[processor.getSampleIndex()] = p;
    }
}

void WaveThumbnail::setColours(Colour& bg, Colour& mid, Colour& dark, Colour& mod, Colour& modulator)
{
    bgColour = bg;
    midColour = mid;
    darkColour = dark;
    modColour = mod;
    modulatorColour = modulator;
}

