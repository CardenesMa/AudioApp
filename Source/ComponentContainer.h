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
#include "Components/VibratoComponent.h"
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
	Visualizer viz;
	VibratoComponent vib;

	ComboBox* xCombo;

	std::vector<ComboBox*> axes = { &glob.x_choice, &glob.y_choice, &glob.z_choice };



	//====== the following are variables for making things look nice =====
	int padding = Style::padding;
	int component_spacing = Style::spacing;


	MainComponent() :
		viz(&glob.x_choice, &glob.y_choice, &glob.z_choice),
		vib(&glob.x_choice, &glob.y_choice, &glob.z_choice)
	{

		setOpaque(true);

		xCombo = &glob.x_choice;

		addAndMakeVisible(glob);
		addAndMakeVisible(viz);
		addAndMakeVisible(vib);

		setSize(width, height);

		// do this for debugging if you want to see the midi messgaes that come through
		//seeMessages();


	}
	~MainComponent() {
		// let go of the device when closed
	}


	void paint(Graphics& g) override {
		g.fillAll(Style::background);
		//handleComboBoxOption(xCombo

		// this is incredibly time inefficient. 
		// for the future, re
		for (ComboBox* ax : axes) {
			int index = ax->getSelectedId();
			switch (index) {
			case 1: handlePitchBend();
			}
		}
	}


	void resized() override {
		// right aligned 
		viz.setBounds(width - viz.width - padding, padding, viz.width, viz.height);
		// top left aligned
		glob.setBounds(padding, padding, glob.width, glob.height);
		// aligned with relation to glob
		vib.setBounds(padding, glob.height + padding + component_spacing, vib.width, vib.height);
	}




	void handlePitchBend() {
		vib.handlePitchBend();
	}

	void handleVibratoFrequency() {}



	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)

};