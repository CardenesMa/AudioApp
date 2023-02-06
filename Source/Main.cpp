

#include <JuceHeader.h>
//#include "AudioAppDemo.h"
//#include "MPosDials.h"

#include "Style.h"
#include "ComponentContainer.h"
#include "Components/Dial.h"
#include "Components/Visualizer.h"
#include "Components/GlobalComponent.h"
#include "Components/VibratoComponent.h"

class Application : public juce::JUCEApplication
{
public:
	//==============================================================================
	Application() = default;

	const juce::String getApplicationName() override { return "Audio VAT"; }
	const juce::String getApplicationVersion() override { return "1.0.0"; }

	void initialise(const juce::String&) override
	{
		mainWindow.reset(new MainWindow("My MIDI", new MainComponent, *this));

	}

	void shutdown() override { mainWindow = nullptr; }

private:
	class MainWindow : public juce::DocumentWindow
	{
	public:
		MainWindow(const juce::String& name, juce::Component* c, JUCEApplication& a)
			: DocumentWindow(name, juce::Desktop::getInstance().getDefaultLookAndFeel()
				.findColour(ResizableWindow::backgroundColourId),
				juce::DocumentWindow::allButtons),
			app(a)
		{
			setUsingNativeTitleBar(true);
			setContentOwned(c, true);

#if JUCE_ANDROID || JUCE_IOS
			setFullScreen(true);
#else
			// set this to (true, false) for debugging. otherwise, keep fixed
			setResizable(true, false);
			//setResizeLimits(300, 250, 10000, 10000);
			centreWithSize(getWidth(), getHeight());
#endif

			setVisible(true);
		}

		void closeButtonPressed() override
		{
			app.systemRequestedQuit();
		}

	private:
		JUCEApplication& app;

		//==============================================================================
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
	};

	std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
START_JUCE_APPLICATION(Application)