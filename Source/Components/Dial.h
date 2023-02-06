# pragma once
#include "./style.h"
// this creates a dial that is in absolute position of the parent that calls it.
/*
class DialLookAndFeel : public LookAndFeel_V4 {
public:
	void drawRotarySlider(Graphics& g, int x, int y, int height, int width, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, Slider& slider) override {
		float diameter = jmin(width, height);
		float radius = diameter / 2;
		float centerX = x + width / 2;
		float centerY = y + height / 2;
		float rx = centerX - radius;
		float ry = centerY - radius;
		float theta = rotaryStartAngle + (sliderPos * (rotaryEndAngle - rotaryStartAngle));


		Rectangle<float> dialArea(rx, ry, diameter, diameter);

		g.setColour(Style::button);
		g.fillEllipse(dialArea);

		g.setColour(Style::accent);
		Path dialTick;
		dialTick.addRectangle(dialArea);

		g.fillPath(dialTick, AffineTransform::rotation(theta).translated(centerX, centerY));

		g.setColour(Style::background);
		g.drawEllipse(rx, ry, diameter, diameter, 5.0f);
	}
};
*/

class Dial : public Component {
public:

	Slider dial;
	Label label;
	//DialLookAndFeel styling;

	int value = 1;


	Dial(String name, int max_value, int steps) {
		addAndMakeVisible(dial);
		dial.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);

		dial.setRange(0, max_value, int(max_value / steps));
		dial.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, getWidth(), 20);

		addAndMakeVisible(label);
		label.setText(name, dontSendNotification);
		label.attachToComponent(&dial, false);
		label.setJustificationType(Justification::horizontallyCentred);

		dial.setValue(value);


	}


	void resized() override {

		label.setBounds(0, 0, getWidth(), getHeight() * 0.3);
		dial.setBounds(0, getHeight() * 0.3, getWidth(), getHeight());
	}

	void paint(Graphics& g) override {

	}
};
