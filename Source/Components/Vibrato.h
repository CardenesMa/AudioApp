#pragma once
#include "Dial.h"

class Vibrato : public Component, private KeyListener {
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

	// dials
	Dial ampDial;
	Dial freqDial;

	float freq = 1.0;
	int ampl = 16;
	float oscillations;

	std::vector<ComboBox*> axes;


	Vibrato(ComboBox* x, ComboBox* y, ComboBox* z) :
		ampDial("Amplitude", height / 2, 127),
		freqDial("Frequency", 30, 127)
	{
		xAxisChoice = x;
		yAxisChoice = y;
		zAxisChoice = z;

		axes = { xAxisChoice, yAxisChoice, zAxisChoice };

		addAndMakeVisible(ampDial);
		addAndMakeVisible(freqDial);

		ampDial.dial.onValueChange = [this] {this->ampl = ampDial.dial.getValue(); repaint(); };
		freqDial.dial.onValueChange = [this] {this->freq = freqDial.dial.getValue(); repaint(); };


		setOpaque(true);

		// for debugging 
		setWantsKeyboardFocus(true);
		addKeyListener(this);

	}
	~Vibrato() override {}



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
		int dial_size = width / 5;
		ampDial.setBounds(width / 4, height / 2 + margin, dial_size, dial_size);
		freqDial.setBounds(width / 2, height / 2 + margin, dial_size, dial_size);
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
	void handleVibratoAmplitude(int value) {
		ampl = value;
		ampDial.dial.setValue(value);
		repaint();
	}
	void handleVibratoFrequency(int value) {
		float mapped_val = map(value, 0, 127, 0, 1000) / 100;
		freq = mapped_val;
		freqDial.dial.setValue(freq);

		repaint();
	}

};
