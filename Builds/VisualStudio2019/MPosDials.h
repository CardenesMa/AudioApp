#pragma once

class MPosDials : public AudioAppComponent {
public:
	MPosDials() {
		setSize(800, 600);
	}
	~MPosDials() override {
		shutdownAudio();
	}
};