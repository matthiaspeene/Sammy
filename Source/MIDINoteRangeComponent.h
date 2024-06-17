#include <JuceHeader.h>

class MIDINoteRangeComponent : public juce::Component
{
public:
    MIDINoteRangeComponent(SammyAudioProcessor& p)
        : processor(p),
        bgColour(p.getBgColour()),
        midColour(p.getMidColour()),
        darkColour(p.getDarkColour()),
        modColour(p.getModColour()),
        modulatorColour(p.getModulatorColour())
    {
        // Configure two-value slider for range selection
        midiRangeSlider.setSliderStyle(juce::Slider::TwoValueHorizontal);
        midiRangeSlider.setRange(0, 127);
        midiRangeSlider.setMinAndMaxValues(0, 127);
        midiRangeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        midiRangeSlider.setColour(juce::Slider::trackColourId, modulatorColour);
        midiRangeSlider.setColour(juce::Slider::thumbColourId, modColour);
        midiRangeSlider.onValueChange = [this] { updateActiveNotes(); };

        // Configure single-value slider for root note selection
        rootNoteSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        rootNoteSlider.setRange(0, 127);
        rootNoteSlider.setValue(60);  // Default C4
        rootNoteSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        rootNoteSlider.setColour(juce::Slider::thumbColourId, modColour);
        rootNoteSlider.onValueChange = [this] { updateRootNote(); };  // Repaint to update root note highlighting

        // Add components
        addAndMakeVisible(midiRangeSlider);
        midiRangeSlider.setAlpha(0.f);
        addAndMakeVisible(rootNoteSlider);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(bgColour);
        drawMidiKeyboard(g);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(10);
        midiRangeSlider.setBounds(area.removeFromTop(area.getHeight()/2));
        rootNoteSlider.setBounds(area.removeFromRight(area.getWidth()/2));
    }

    void updateSettings()
    {
        juce::BigInteger midiNotes = processor.getMidiNotes();
        int minNote = midiNotes.findNextSetBit(0);
        int maxNote = midiNotes.findNextClearBit(minNote);

        if (minNote != -1 && maxNote != -1 && minNote <= maxNote)
        {
            midiRangeSlider.setMinAndMaxValues(minNote, maxNote);
        }

        rootNoteSlider.setValue(processor.getMidiRootNote());

        updateActiveNotes();
        updateRootNote();
    }

private:
    SammyAudioProcessor& processor;
    juce::BigInteger activeMidiNotes;
    juce::Slider midiRangeSlider;
    juce::Slider rootNoteSlider;

    // Color configuration from processor
    juce::Colour bgColour, midColour, darkColour, modColour, modulatorColour;

    void drawMidiKeyboard(juce::Graphics& g)
    {
        auto area = getLocalBounds().reduced(12);

        int numKeys = 75; // Number of keys to display for simplicity
        int startKey = 24;
        int totalWhiteKeys = countWhiteKeys(startKey, numKeys);
        int keyWidth = area.getWidth() / totalWhiteKeys;
        int keyHeight = area.removeFromTop(area.getHeight()/2).getHeight();
        int offset = 12; 

        for (int i = 0; i < numKeys; ++i)
        {
            int note = startKey + i;
            bool isBlack = juce::MidiMessage::isMidiNoteBlack(note);
            bool isC = (juce::MidiMessage::getMidiNoteName(note,false,false,3) == "C");
            bool isRootNote = (note == static_cast<int>(rootNoteSlider.getValue()));

            if (!isBlack) {

                g.setColour(isRootNote ? modColour : darkColour);
                isRootNote ? g.fillRect(offset, 12, keyWidth, keyHeight) : g.drawRect(offset, 12, keyWidth, keyHeight);

                if (isC) {
                    juce::String octaveLabel = "C";
                    g.setColour(darkColour);
                    g.drawText(octaveLabel, offset, keyHeight-10, 1, 1, juce::Justification::centred, false);
                }
                offset += keyWidth;

            }
            else {
                // Draw white keys
                g.setColour(isRootNote ? modColour : darkColour);
                int blackKeyWidth = keyWidth / 2;
                int blackKeyOffset = offset - blackKeyWidth / 2; 
                g.fillRect(blackKeyOffset, 12, blackKeyWidth, keyHeight*0.6);


            }

            if (note >= midiRangeSlider.getMinValue()-1 && note <= midiRangeSlider.getMaxValue() && !isRootNote) {
                g.setColour(modulatorColour.withAlpha(0.5f));
                int highlightTop = isBlack ? 12: 12;
                int highlightHeight = isBlack ? keyHeight*0.6 : keyHeight;
                int highlightOffset = isBlack ? (offset - keyWidth / 2) : (offset - keyWidth);
                g.fillRect(highlightOffset, highlightTop, keyWidth - 1, highlightHeight);
            }

            juce::String lowestKey = juce::MidiMessage::getMidiNoteName(midiRangeSlider.getMinValue(), true, true, 4);
            juce::String highestKey = juce::MidiMessage::getMidiNoteName(midiRangeSlider.getMaxValue(), true, true, 4);
            juce::String rootNote = juce::MidiMessage::getMidiNoteName(rootNoteSlider.getValue(), true, true, 4);

            g.setColour(darkColour);
            g.drawText("Midirange: " + lowestKey + "-" + highestKey, area.getX(), area.getY(), area.getWidth() * 0.6, area.getHeight()/2, juce::Justification::topLeft, false);
            g.drawText("Rootnote: " + rootNote, area.getX(), area.getY() + area.getHeight() / 2, area.getWidth() * 0.6, area.getHeight()/2, juce::Justification::topLeft, false);
        }
    }

    void updateActiveNotes()
    {
        activeMidiNotes.clear();
        auto startNote = static_cast<int>(midiRangeSlider.getMinValue());
        auto endNote = static_cast<int>(midiRangeSlider.getMaxValue());
        activeMidiNotes.setRange(startNote, endNote - startNote + 1, true);
        processor.updateActiveMidiNotes(activeMidiNotes); // Update the processor's MIDI settings
    }

    void updateRootNote()
    {
        processor.updateRootNote(rootNoteSlider.getValue());
    }

    int countWhiteKeys(int startKey, int numKeys) {
        int count = 0;
        for (int i = 0; i < numKeys; ++i) {
            int note = startKey + i;
            if (!juce::MidiMessage::isMidiNoteBlack(note)) {
                count++;
            }
        }
        return count;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDINoteRangeComponent)
};
