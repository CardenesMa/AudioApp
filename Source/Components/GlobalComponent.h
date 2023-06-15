#pragma once

#include "../Style.h"
#include "Dial.h"

class GlobalComponent : public Component, private MidiInputCallback {
public:
	// detect devices and connect etc

	AudioDeviceManager deviceManager;
	ComboBox midiInputList;

	MidiMessage current_message;


	ComboBox x_choice;
	ComboBox y_choice;
	ComboBox z_choice;

	TextButton zeroButton;

	Label xLabel;
	Label yLabel;
	Label zLabel;
	Label inputLabel;

	StringArray axisChoices = Style::ComboBoxOptions;


	int lastInputIndex = 0;
	bool isAddingFromMidiInput = false;

	double startTime;

	int width = Style::GlobalBoxSize[0];
	int height = Style::GlobalBoxSize[1];

	int margin = Style::GlobalBoxSize[2];



	GlobalComponent()
		:startTime(Time::getMillisecondCounterHiRes() * 0.01)
	{
		midiInputList.setTextWhenNoChoicesAvailable("Nothing available :(");
		// styling for the input list
		midiInputList.setColour(ComboBox::backgroundColourId, Colour(0x3fcbe6ff));

		// see what devices are available
		auto midiInputs = MidiInput::getAvailableDevices();

		StringArray midiInputNames;
		// add each available device to the dropdown
		for (MidiDeviceInfo input : midiInputs) midiInputNames.add(input.name);
		midiInputList.addItemList(midiInputNames, 1);
		// make sure to handle change
		midiInputList.onChange = [this] {setMidiInput(midiInputList.getSelectedItemIndex()); };

		// use a certain device for the inputs
		for (auto input : midiInputs) {
			if (deviceManager.isMidiInputDeviceEnabled(input.identifier)) {
				setMidiInput(midiInputs.indexOf(input));
				break;
			}
		}

		if (midiInputList.getSelectedId() == 0) setMidiInput(0);

		addAndMakeVisible(midiInputList);


		// == begin the axis boxes ==
		auto axes = { &x_choice, &y_choice, &z_choice };
		for (auto i : axes) {
			// visibility
			addAndMakeVisible(*i);
			i->addItemList(axisChoices, 1);
			// styling of our axes boxes
			i->setColour(ComboBox::backgroundColourId, Style::button);
			i->setColour(ComboBox::outlineColourId, Style::outline);
			i->setColour(ComboBox::arrowColourId, Style::text);
			i->setColour(ComboBox::textColourId, Style::text);

		}
		// == end boxes ==
		// == begin labels ==

		xLabel.setText("X Axis Controller", dontSendNotification);
		xLabel.attachToComponent(&x_choice, true);

		yLabel.setText("Y Axis Controller", dontSendNotification);
		yLabel.attachToComponent(&y_choice, true);

		zLabel.setText("Z Axis Controller", dontSendNotification);
		zLabel.attachToComponent(&z_choice, true);

		addAndMakeVisible(inputLabel);
		inputLabel.setText("MIDI Input: ", dontSendNotification);
		inputLabel.attachToComponent(&midiInputList, true);

		auto labels = { &xLabel, &yLabel, &zLabel, &inputLabel };
		for (auto i : labels) {
			i->setColour(ComboBox::outlineColourId, Style::outline);
			i->setColour(ComboBox::textColourId, Style::text);
		}

		// == end labels ==
		// == begin buttons ==
		addAndMakeVisible(zeroButton);
		zeroButton.setButtonText("Zero Device");
		// == end buttons ==

	}

	~GlobalComponent() override {
		deviceManager.removeMidiInputDeviceCallback(MidiInput::getAvailableDevices()[midiInputList.getSelectedItemIndex()].identifier, this);
	}

	void paint(Graphics& g) override {
		g.fillAll(Colour(Style::background));

		g.setColour(Style::componentBackground);
		g.fillRoundedRectangle(0, 0, width, height, margin);
	}

	void resized() override {
		auto area = getLocalBounds();
		int input_list_height = 50;

		midiInputList.setBounds(margin, margin, 100, 50);

		int label_width = 100;
		int label_height = 20;
		int choice_box_height = 30; // this is the size of the floating boxes (y direction. currently a 1:3 ratio height:width)
		float choice_box_width = choice_box_height * 1.618 * 2; // golden ratio! (x2 because doesn't fit)


		xLabel.setBounds(margin, margin * 2 + input_list_height, label_width, label_height);
		yLabel.setBounds(margin, margin * 3 + input_list_height + choice_box_height, label_width, label_height);
		zLabel.setBounds(margin, margin * 4 + input_list_height + choice_box_height * 2, label_width, label_height);

		x_choice.setBounds(label_width + margin * 2, margin * 2 + input_list_height, choice_box_width, choice_box_height);
		y_choice.setBounds(label_width + margin * 2, margin * 3 + input_list_height + choice_box_height, choice_box_width, choice_box_height);
		z_choice.setBounds(label_width + margin * 2, margin * 4 + input_list_height + choice_box_height * 2, choice_box_width, choice_box_height);

		int z_bt_width = 150;
		int z_bt_height = 50;
		zeroButton.setBounds(width - margin - z_bt_width, margin, z_bt_width, z_bt_height);

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


	void zeroDevice() {
		std::cout << "Device Zeroed";
		// make the button change colour or something for a moment
	}

	/*int getBoxSelectedIndex(char axis) {
		if (axis == 'x') {
			return x_choice.getSelectedItemIndex();
		}
		else if (axis == 'y') {
			return y_choice.getSelectedItemIndex();
		}
		else if (axis == 'z') {
			return z_choice.getSelectedItemIndex();
		}
		else {
			return -1;
		}

	}*/

	void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message) override {
		current_message = message;
	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GlobalComponent)

private:

};