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

#include "Components/Visualizer.h"
#include "Components/GlobalComponent.h"
#include "Components/Vibrato.h"
#include "Style.h"


class MainComponent : public Component {
public:
	// sizing options
	int width = Style::MainWindowSize[0];
	int height = Style::MainWindowSize[1];
	const int margin = Style::MainWindowSize[2];

	// the box which contains the visualizer in it 
	// the box which has all the important dropdowns n stuff in it
	GlobalComponent glob;
	Visualizer VisualizerComponent;
	Vibrato VibratoComponent;


	// 
	AudioDeviceManager* deviceManager = &glob.deviceManager;
	// since the midi listener is in the glob object, let's point to the message so it is continuously updated
	MidiMessage* recieved_midi = &(glob.current_message);
	// store the axes for easy use
	std::vector<ComboBox*> axes = { &glob.x_choice, &glob.y_choice, &glob.z_choice };

	//====== the following are variables for making things look nice =====
	int padding = Style::padding;
	int component_spacing = Style::spacing;

	Array<MidiDeviceInfo> midiOutputs = MidiOutput::getAvailableDevices();
	std::unique_ptr<MidiOutput> output;


	MainComponent() :
		VisualizerComponent(&glob.x_choice, &glob.y_choice, &glob.z_choice),
		VibratoComponent(&glob.x_choice, &glob.y_choice, &glob.z_choice)
	{


		addAndMakeVisible(glob);
		addAndMakeVisible(VisualizerComponent);
		addAndMakeVisible(VibratoComponent);

		setSize(width, height);

		// do this for debugging if you want to see the midi messgaes that come through
		//seeMessages();


		// REVISIT THIS TO AKE SURE THE OUTPUT DEVICE IS CORRECT

		output = MidiOutput::openDevice(midiOutputs[0].identifier);

		sendcc(0x01, 0x00);


	}
	~MainComponent() {
		// let go of the device when closed
	}
	void paint(Graphics& g) override {
		g.fillAll(Style::background);


		handleMidiMessage(*recieved_midi);


	}

	void resized() override {
		// top left aligned
		glob.setBounds(padding, padding, glob.width, glob.height);
		// aligned with relation to glob
		VibratoComponent.setBounds(padding, glob.height + padding + component_spacing, VibratoComponent.width, VibratoComponent.height);

		VisualizerComponent.setBounds(padding * 2 + glob.width, padding, VisualizerComponent.width, VisualizerComponent.height);

		Style::update(getWidth(), getHeight(), 40);
	}



	void handleMidiMessage(const MidiMessage& m) {
		// OK THIS IS SUPER JENK -- 
		// if there is a note on message SENT FROM THE CONTROLLER, then it resets
		if (m.isNoteOn()) handleZeroDevice();
		if (!m.isController()) return;

		auto value = m.getControllerValue();

		for (int i = 0; i < axes.size(); i++) {
			ComboBox* ax = axes[i];


			// do all the conditionals here so that I don't have to check in each case
			bool matches_controller_number = m.getControllerNumber() == 103 + i;
			if (!matches_controller_number) continue;

			// visualize the coordinate with respect to the axis
			VisualizerComponent.setCoordinate(value, 127, 1 + m.getControllerNumber() - 103);


			switch (ax->getSelectedItemIndex()) {
			case 0: handlePitchBend(value); break;
			case 1: handleCC01(value); break;
			case 2: handleCC70(value); break;
			case 3: handleCC74(value); break;
			case 4: handleVibratoAmplitude(value); break;
			case 5: handleVibratoFrequency(value); break;
			default:
				handleOtherwise(value);
			}
		}
		//since this method is dependent upon painting being done, let it be circular so that it continues to execute itself when ready
		repaint();
	}

	bool sendcc(uint8_t ccnumber, uint8_t value) {
		// note that for all uses of the value, it is =<127, therefore usable in uint8_t
		uint8_t status = 0b10110000;
		MidiMessage m(status, ccnumber, value);
		output.get()->sendMessageNow(m);
		return true;

	}

	bool sendpitchbend(uint16_t value) {
		uint8_t status = 0xe0;
		MidiMessage m(status, value);
		/*if (midiOutputs[0]->outDevice.get() != nullptr)
			midiOutputs[0]->outDevice->sendMessageNow(m);*/
		return true;
	}
private:
	void handleZeroDevice() {
		glob.zeroDevice();

	}

	void handlePitchBend(int value) {

	}

	void handleCC01(int value) {
		// usually vibato or something but sometimes tire, resonsnace, etc.
		sendcc(0x01, value);
	}

	void handleCC70(int value) {
		sendcc(0x07, value);
	}

	void handleCC74(int value) {
		sendcc(0x16, value);
	}

	void handleVibratoAmplitude(int value) {
		// create an LFO or sinusodal method to handle this with pitch bend so that works with 
		// Digital instruments that can't handle the vibrato information
		//sendcc(0x01, value);
		VibratoComponent.handleVibratoAmplitude(value);
	}

	void handleVibratoFrequency(int value) {
		VibratoComponent.handleVibratoFrequency(value);
	}

	void handleOtherwise(int value) {}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};