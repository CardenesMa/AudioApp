#pragma once
#include "Dial.h"

class VibratoComponent : public Component, private MidiInputCallback, private KeyListener {
public:
	// I'm using these so that if the choices on the global comboboxes are 
	// related to vibrato, it can be controlled here 
	ComboBox* xAxisChoice;
	ComboBox* yAxisChoice;
	ComboBox* zAxisChoice;

	bool isAddingFromMidiInput = false;

	int width = Style::VibratoBoxSize[0];
	int height = Style::VibratoBoxSize[1];
	int margin = Style::VibratoBoxSize[2];

	int graph_height = height / 2;
	int graph_width = width - margin * 2;


	//Dial freqDial;
	//Slider freqDial;

	//double* freq;
	int freq = 1;
	int ampl = 20;
	float oscillations;

	std::vector<ComboBox*> axes;

	VibratoComponent(ComboBox* x, ComboBox* y, ComboBox* z) {
		xAxisChoice = x;
		yAxisChoice = y;
		zAxisChoice = z;

		axes = { xAxisChoice, yAxisChoice, zAxisChoice };

		//addAndMakeVisible(freqDial);


		setOpaque(true);

		// for debugging 
		setWantsKeyboardFocus(true);
		addKeyListener(this);

		//for (ComboBox* ax : axes) {
		//	switch (ax->getSelectedItemIndex()) {
		//	case 0: // Pitch Bend (see Style.h for reference for all indicides
		//		handlePitchBend();
		//	case 1:
		//		handleCC1();
		//	}
		//}







	}
	~VibratoComponent() override {}
	String getMidiMessageDescription(const MidiMessage& m) {

		repaint();
	}


	void paint(Graphics& g) override {
		// this line is important since otherwise you can see the trimmed edges of the rectangle
		g.fillAll(Colour(Style::background));

		g.setColour(Style::componentBackground);
		g.fillRoundedRectangle(0, 0, width, height, margin);

		Colour axes_colour(Style::axes);
		g.setColour(axes_colour);

		//make the axes [ |- shape ]
		// x -- 
		g.drawLine(margin, margin + graph_height / 2, width - margin, margin + graph_height / 2);
		// y |
		g.drawLine(margin, margin, margin, margin + graph_height);

		g.setColour(Style::accent);
		// make a sinusodal path relating to vibrato
		Path sinusoid;
		sinusoid.startNewSubPath(margin, margin + graph_height / 2);

		// going right from the origin
		for (float i = 0; i <= graph_width; i += 0.1) {
			double f_y = ampl * std::sin(freq * 2 * 3.14 * i / graph_width);
			Point<float> loc(margin + i, margin + graph_height / 2 + f_y);

			if (loc.y < margin) loc.y = margin;
			if (loc.y > margin + graph_height) loc.y = margin + graph_height;

			sinusoid.lineTo(loc);
		}

		g.strokePath(sinusoid, PathStrokeType(1.0f));

	}
	void resized() override {
		//freqDial.setBounds(100, 150, freqDial.width, freqDial.height);
	}

	void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message) override
	{
		const juce::ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
	}


	bool keyPressed(const KeyPress& k, Component* c) override {
		if (k.getTextCharacter() == 'a') {
			freq--;
			repaint();
			return true;
		}
		else if (k.getTextCharacter() == 'd') {
			freq++;
			repaint();
			return true;
		}
		else if (k.getTextCharacter() == 'w') {
			ampl += 2;
			repaint();
			return true;
		}
		else if (k.getTextCharacter() == 's') {
			ampl -= 2;
			repaint();
			return true;
		}
	}


	void handlePitchBend() {

	}

	void handleCC01() {}
	void handleCC70() {}
	void handleCC74() {}
	void handleVibratoAmplitude() {

	}
	void handleVibratoFrequency() {}

};
