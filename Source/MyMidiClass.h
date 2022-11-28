/*
  ==============================================================================

	MyMidiClass.h
	Created: 24 Nov 2022 3:24:49pm
	Author:  Cardenes


	What I'm making : a graph which plots two messages (pitch bend, CC) based on some MIDI input
  ==============================================================================
*/

//  https://docs.juce.com/master/tutorial_component_parents_children.html


#pragma once

int map(int x, int in_min, int in_max, int out_min, int out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


class MainComponent : public Component, private MidiInputCallback {

public:
	// I use this to free up memory when we've reached a cap
	int message_number = 0;
	double startTime;
	// detect devices and connect etc
	AudioDeviceManager deviceManager;
	// make the box and label for seeing the midi devices
	ComboBox midiInputList;
	ComboBox xAxisChoice;
	ComboBox yAxisChoice;
	ComboBox zAxisChoice;

	StringArray axisChoices = { String("Pitch Bend"), String("CC70"), String("CC74"), String("CC01") };

	int lastInputIndex = 0;
	bool isAddingFromMidiInput = false;


	TextEditor midiMessagesBox;
	// margin for the axes
	const int margin = 40;
	int height = 500;
	int width = 500;

	float x_cor = margin;
	float y_cor = height - margin;
	// rgb, radius
	Array<int> z_cor = { 255,0,0, 5 };

	MainComponent()
		: startTime(Time::getMillisecondCounterHiRes() * 0.01) {
		setOpaque(true);




		// this is the dropdown menu that displays the available midi controllers (currently top-left corner, 11-25)
		addAndMakeVisible(midiInputList);
		midiInputList.setTextWhenNoChoicesAvailable("Nothing available :(");
		auto midiInputs = MidiInput::getAvailableDevices();

		StringArray midiInputNames;
		// add each available device to the dropdown
		for (MidiDeviceInfo input : midiInputs) midiInputNames.add(input.name);

		midiInputList.addItemList(midiInputNames, 1);
		midiInputList.onChange = [this] {setMidiInput(midiInputList.getSelectedItemIndex()); };

		// use a certain device for the inputs
		for (auto& input : midiInputs) {
			if (deviceManager.isMidiInputDeviceEnabled(input.identifier)) {
				setMidiInput(midiInputs.indexOf(input));
				break;
			}
		}

		if (midiInputList.getSelectedId() == 0) setMidiInput(0);

		// === end of midi input list ===

		// these are the dropdown labels that assign the axes to a midi input
		addAndMakeVisible(xAxisChoice);
		addAndMakeVisible(yAxisChoice);
		addAndMakeVisible(zAxisChoice);

		zAxisChoice.addItemList(axisChoices, 1);
		xAxisChoice.addItemList(axisChoices, 1);
		yAxisChoice.addItemList(axisChoices, 1);


		setSize(width, height);

		// do this for debugging if you want to see the midi messgaes that come through
		//seeMessages();
	}
	~MainComponent() override {
		// let go of the device when closed
		deviceManager.removeMidiInputDeviceCallback(MidiInput::getAvailableDevices()[midiInputList.getSelectedItemIndex()].identifier, this);
	}

	String getMidiMessageDescription(const juce::MidiMessage& m)
	{
		// check to see what the curent value of the axes are, moving the coordinate as needed

		// ==== fo shizle-- make this enumed or something. these many case statements as they increase will be too much
		switch (xAxisChoice.getSelectedItemIndex()) {
		case 0:
			if (m.isPitchWheel()) setCoordinate(m.getPitchWheelValue(), 16383, 1);
			break;
		case 1:
			if (m.isController() && m.getControllerNumber() == 70) setCoordinate(m.getControllerValue(), 127, 1);
			break;
		case 2:
			if (m.isController() && m.getControllerNumber() == 74) setCoordinate(m.getControllerValue(), 127, 1);
			break;
		case 3:
			if (m.isController() && m.getControllerNumber() == 1) setCoordinate(127 - m.getControllerValue(), 127, 1);
			break;

		}
		switch (yAxisChoice.getSelectedItemIndex())
		{
		case 0:
			if (m.isPitchWheel()) setCoordinate(m.getPitchWheelValue(), 16383, 2);
			break;
		case 1:
			if (m.isController() && m.getControllerNumber() == 70) setCoordinate(m.getControllerValue(), 127, 2);
			break;
		case 2:
			if (m.isController() && m.getControllerNumber() == 74) setCoordinate(m.getControllerValue(), 127, 2);
			break;
		case 3:
			if (m.isController() && m.getControllerNumber() == 1) setCoordinate(127 - m.getControllerValue(), 127, 2);

		}
		switch (zAxisChoice.getSelectedItemIndex())
		{
		case 0:
			if (m.isPitchWheel()) setCoordinate(m.getPitchWheelValue(), 16383, 3);
			break;
		case 1:
			if (m.isController() && m.getControllerNumber() == 70) setCoordinate(m.getControllerValue(), 127, 3);
			break;
		case 2:
			if (m.isController() && m.getControllerNumber() == 74) setCoordinate(m.getControllerValue(), 127, 3);
			break;
		case 3:
			if (m.isController() && m.getControllerNumber() == 1) setCoordinate(m.getControllerValue(), 127, 3);

		}

		repaint();

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
			//===== make this dynamic later so that we can change the controller number option on the axes ====




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
		midiMessagesBox.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x32ffffff));
		midiMessagesBox.setColour(juce::TextEditor::outlineColourId, juce::Colour(0x1c000000));
		midiMessagesBox.setColour(juce::TextEditor::shadowColourId, juce::Colour(0x16000000));


	}

	void paint(Graphics& g) override {
		g.fillAll(Colours::black);

		g.setColour(Colours::white);
		//make the axes
		//x --
		g.drawLine(margin, height - margin, width - margin, height - margin);
		// y |
		g.drawLine(margin, margin, margin, height - margin);


		// draw the point (changes colour in the z direction for now since I don't wanna do 3d visualizing trig)
		int point_radius = z_cor[3];
		g.setColour(Colour(z_cor[0], z_cor[1], z_cor[2]));
		g.fillEllipse(x_cor - point_radius, y_cor - point_radius, point_radius * 2, point_radius * 2);



	}



	void resized() override {
		height = getHeight();
		width = getWidth();

		auto area = getLocalBounds();
		// resize the stuff when the window changes
		midiMessagesBox.setBounds(area.reduced(8));
		midiInputList.setBounds(area.removeFromTop(40).removeFromRight(150).reduced(8));

		int choice_box_size = 30; // this is the size of the floating boxes (y direction. currently a 1:3 ratio height:width)

		xAxisChoice.setBounds(2 * choice_box_size / 3, (margin - choice_box_size) / 2, choice_box_size * 3, choice_box_size);
		yAxisChoice.setBounds(width - (margin - choice_box_size) - choice_box_size * 3, height - margin - choice_box_size / 2, choice_box_size * 3, choice_box_size);
		zAxisChoice.setBounds(height / 2, width / 2, choice_box_size * 3, choice_box_size);
	}

	void setMidiInput(int index) {
		auto list = MidiInput::getAvailableDevices();
		// get rid of the previous midi device and listen to the new one
		deviceManager.removeMidiInputDeviceCallback(list[lastInputIndex].identifier, this);

		auto newInput = list[index];

		if (!deviceManager.isMidiInputDeviceEnabled(newInput.identifier))
			deviceManager.setMidiInputDeviceEnabled(newInput.identifier, true);
		// begin the callbacks to here
		deviceManager.addMidiInputDeviceCallback(newInput.identifier, this);
		midiInputList.setSelectedId(index + 1, dontSendNotification);
		lastInputIndex = index;
	}

	void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message) override
	{
		const juce::ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
		postMessageToList(message, source->getName());
	}

	// dispatch an incoming message to the mesasge thread. Not entirely sure what this is for
	class IncomingMessageCallback : public CallbackMessage {
	public:
		IncomingMessageCallback(MainComponent* o, const MidiMessage& m, const String& s)
			: owner(o), message(m), source(s)
		{}

		void messageCallback() override {
			if (owner != nullptr)
				owner->addMessageToList(message, source);
		}
		Component::SafePointer<MainComponent> owner;
		MidiMessage message;
		String source;
	};

	// 
	void postMessageToList(const MidiMessage& message, String& source) {
		// looks like it creates a new incoming message object and posts it to some board??
		(new IncomingMessageCallback(this, message, source))->post();
	}

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


	// ==== set the coordinate of an axis based on the value and the maximum potential (127 usually, 16383 for pirch bend)
	void  setCoordinate(int value, int max_potential_input_value, int axis) {
		if (axis == 1) x_cor = map(value, 0, max_potential_input_value, margin, width - margin);
		if (axis == 2) y_cor = height - map(value, 0, max_potential_input_value, margin, height - margin);
		if (axis == 3) z_cor = { 255 - map(value, 0, max_potential_input_value, 0, 255), 0, map(value, 0, max_potential_input_value, 0, 255),  map(value, 0, max_potential_input_value, 2, 50) };

	}


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)

};