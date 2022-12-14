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
	const Colour background{ 0xffB39C4D };
	const Colour outline{ 0xff000000 };
	const Colour text{ 0xffffffff };
	const Colour componentBackground{ 0xff768948 };
	const Colour accent{ 0xff55D6BE };
	const Colour button{ 0xff607744 };
	const Colour axes{ 0xff34623F };
	const Colour guideline{ 0xffEFCEFA };

	// == Spacings ==
	const int padding = 5;
	const int spacing = 10;

	// == Sizes == (width, height, margin)
	const std::vector<int> MainWindowSize = { 800,400,40 };
	const std::vector<int> VisualizerBoxSize = { MainWindowSize[0] / 2 - padding * 2 - spacing, MainWindowSize[1] - padding * 2,                   10 };
	const std::vector<int> GlobalBoxSize = { MainWindowSize[0] / 2 - padding * 2 - spacing,  MainWindowSize[1] / 2 - padding * 2 - spacing / 2, 10 };
	const std::vector<int> VibratoBoxSize = { MainWindowSize[0] / 2 - padding * 2 - spacing, MainWindowSize[1] / 2 - padding * 2 - spacing / 2, 10 };

	const int dialRadius = 10;
}
