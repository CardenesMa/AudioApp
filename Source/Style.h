#pragma once

namespace Style {
	// == ComboBox Information == 
	StringArray ComboBoxOptions = {
		"Pitch Bend",
		"CC01",
		"CC70",
		"CC74",
		"Vibrato Amplitude",
		"Vibrato Frequency"
	};
	//== Colours == (0x+alpha+red+green+blue)
	const Colour background{ 0xff404044 };
	const Colour outline{ 0xff000000 };
	const Colour text{ 0xffffffff };
	const Colour componentBackground{ 0xff4d4d53 };
	const Colour accent{ 0xff3ca9dd };
	const Colour button{ 0xff636a6d };
	const Colour axes{ 0xff373737 };
	const Colour guideline{ 0x00EFCEFA };

	// == Spacings ==
	const int padding = 5;
	const int spacing = 10;

	// == Sizes == (width, height, margin)
	std::vector<int> MainWindowSize = { 800,400,40 };
	std::vector<int> GlobalBoxSize = { MainWindowSize[0] / 2 - padding * 2 - spacing,  MainWindowSize[1] / 2 - padding * 2 - spacing / 2, 10 };
	std::vector<int> VibratoBoxSize = { MainWindowSize[0] / 2 - padding * 2 - spacing, MainWindowSize[1] / 2 - padding * 2 - spacing / 2, 10 };
	std::vector<int> VisualizerBoxSize = { MainWindowSize[0] / 2 - padding * 2 - spacing, MainWindowSize[1] - padding * 2, 10 };

	const int dialRadius = 5;

	inline void update(int width, int height, int margin) {
		std::vector<int> MainWindowSize = { width,height,margin };
		std::vector<int> GlobalBoxSize = { MainWindowSize[0] / 2 - padding * 2 - spacing,  MainWindowSize[1] / 2 - padding * 2 - spacing / 2, 10 };
		std::vector<int> VibratoBoxSize = { MainWindowSize[0] / 2 - padding * 2 - spacing, MainWindowSize[1] / 2 - padding * 2 - spacing / 2, 10 };
		std::vector<int> VisualizerBoxSize = { MainWindowSize[0] / 2 - padding * 2 - spacing, MainWindowSize[1] - padding * 2, 10 };
	}
}
