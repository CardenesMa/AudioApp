#pragma once

#include "../Style.h"

int map(int x, int in_min, int in_max, int out_min, int out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


class Visualizer : public Component {
public:
	// make the box and label for seeing the midi devices
	ComboBox* xAxisChoice;
	ComboBox* yAxisChoice;
	ComboBox* zAxisChoice;

	TextEditor midiMessagesBox;

	bool isAddingFromMidiInput = false;
	// I use this to free up memory when we've reached a cap
	int message_number = 0;

	std::vector<ComboBox*> axes;

	// margin for the axes
	int width = Style::VisualizerBoxSize[0];
	int height = Style::VisualizerBoxSize[1];

	int margin = Style::VisualizerBoxSize[2];

	double startTime;



	float x_cor = margin;
	float y_cor = height - margin;
	// rgb, radius
	Array<uint8> z_cor = { 255,0,0, 5 };

	Visualizer(ComboBox* x, ComboBox* y, ComboBox* z)
		:startTime(Time::getMillisecondCounterHiRes() * 0.01) {
		xAxisChoice = x;
		yAxisChoice = y;
		zAxisChoice = z;

		axes = { xAxisChoice, yAxisChoice, zAxisChoice };
		// add all the syling necessary to the axes labels
		setOpaque(true);

	}
	~Visualizer() override {}

	void paint(Graphics& g) override {
		g.fillAll(Colour(Style::background));
		// create the container outliner
		g.setColour(Style::componentBackground);

		g.fillRoundedRectangle(0, 0, width, height, margin);

		Colour axes_colour(Style::axes);

		g.setColour(axes_colour);
		//make the axes [ L shape ]
		//x --
		g.drawLine(margin, height - margin, width - margin, height - margin);
		// y |
		g.drawLine(margin, margin, margin, height - margin);

		// draw the guidelines 

		g.setColour(Style::guideline);

		for (int i = width - margin; i >= margin + 20; i -= 20) {
			g.drawLine(i, margin, i, height - margin);
		}
		for (int j = height - margin; j >= margin + 20; j -= 20) {
			g.drawLine(margin, j, width - margin, j);
		}

		// draw the point (changes colour in the z direction for now since I don't wanna do 3d visualizing trig)
		int point_radius = z_cor[3];
		g.setColour(Colour(z_cor[0], z_cor[1], z_cor[2]));
		g.fillEllipse(x_cor - point_radius, y_cor - point_radius, point_radius * 2, point_radius * 2);

	}

	void resized() override {

		auto area = getLocalBounds();

		midiMessagesBox.setBounds(area.reduced(2));

	}

	// ==== set the coordinate of an axis based on the value and the maximum potential (127 usually, 16383 for pirch bend)
	void  setCoordinate(int value, int max_potential_input_value, int axis) {
		if (axis == 1) x_cor = map(value, 0, max_potential_input_value, margin, width - margin);
		if (axis == 2) y_cor = height - map(value, 0, max_potential_input_value, margin, height - margin);
		if (axis == 3) z_cor = { 255 - map(value, 0, max_potential_input_value, 0,      255), 0, map(value, 0, max_potential_input_value, 0, 255),  map(value, 0, max_potential_input_value, 2, 50) };
	}

	// THe following is for handling midi messages appropriately

	void postMessageToList(const MidiMessage& message, String& source) {
		// looks like it creates a new incoming message object and posts it to some board??
		(new IncomingMessageCallback(this, message, source))->post();
	}
	class IncomingMessageCallback : public CallbackMessage {
	public:
		IncomingMessageCallback(Visualizer* o, const MidiMessage& m, const String& s)
			: owner(o), message(m), source(s)
		{}

		void messageCallback() override {
			if (owner != nullptr)
				owner->addMessageToList(message, source);
		}
		Component::SafePointer<Visualizer> owner;
		MidiMessage message;
		String source;
	};

	void addMessageToList(MidiMessage& message, String& source) {
		if (message_number > 20) {
			midiMessagesBox.clear();
			message_number = 0;
		}
		message_number++;
		auto time = message.getTimeStamp() - startTime;

		auto timecode = String::formatted("%02d:%02d",
			(int)(time / 60.0) % 60,
			(int)time % 60
		);

		auto description = getMidiMessageDescription(message);

		String midiMessageString(timecode + " - " + description);
		logMessage(midiMessageString);
	}

	// logs message to the visualized message box
	void logMessage(const String& m) {
		midiMessagesBox.moveCaretToEnd();
		midiMessagesBox.insertTextAtCaret(m + newLine);
	}

	String getMidiMessageDescription(const juce::MidiMessage& m)
	{


		if (m.isNoteOn())           return "Note on " + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
		if (m.isNoteOff())          return "Note off " + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
		if (m.isProgramChange())    return "Program change " + juce::String(m.getProgramChangeNumber());
		if (m.isPitchWheel())       return "Pitch wheel " + juce::String(m.getPitchWheelValue());
		if (m.isAftertouch())       return "After touch " + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3) + ": " + juce::String(m.getAfterTouchValue());
		if (m.isChannelPressure())  return "Channel pressure " + juce::String(m.getChannelPressureValue());
		if (m.isAllNotesOff())      return "All notes off";
		if (m.isAllSoundOff())      return "All sound off";
		if (m.isMetaEvent())        return "Meta event";
		// here is where the contrrol change messages show up
		if (m.isController())
		{
			String name(MidiMessage::getControllerName(m.getControllerNumber()));

			if (name.isEmpty())
				name = "[" + String(m.getControllerNumber()) + "]";

			return "Controller " + name + ": " + String(m.getControllerValue());
		}

		return String::toHexString(m.getRawData(), m.getRawDataSize());
	}

	void seeMessages() {

		addAndMakeVisible(midiMessagesBox);
		// just a bunch of copypasta parameters to make the dropdown look good
		midiMessagesBox.setMultiLine(true);
		midiMessagesBox.setCentrePosition(width / 2, 2 * height / 3);
		midiMessagesBox.setReturnKeyStartsNewLine(true);
		midiMessagesBox.setReadOnly(true);
		midiMessagesBox.setScrollbarsShown(true);
		midiMessagesBox.setCaretVisible(false);
		midiMessagesBox.setPopupMenuEnabled(true);
		//midiMessagesBox.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x2686dd));
		midiMessagesBox.setColour(juce::TextEditor::outlineColourId, juce::Colour(0x2686dd));
		midiMessagesBox.setColour(juce::TextEditor::shadowColourId, juce::Colour(0x2686dd));

	}




	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Visualizer)

};