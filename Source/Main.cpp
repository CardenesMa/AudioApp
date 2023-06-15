

#include <JuceHeader.h>
#include <windows.h>

#include "ComponentContainer.h"

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
		// open hairless on startup 
		startup("C:/Users/Tino Cardenes/Downloads/hairless-midiserial/hairless-midiserial.exe");


	}

	void shutdown() override { mainWindow = nullptr; }

	void startup(LPCTSTR lpApplicationName)
	{
		// additional information
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		// set the size of the structures
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		// start the program up
		CreateProcess(lpApplicationName,   // the path
			"",        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
		);
		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

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