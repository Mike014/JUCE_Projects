#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
	This component lives inside our window, and this is where you should put all
	your controls and content.
*/
class MainComponent : public juce::AudioAppComponent
{
public:
	//==============================================================================
	MainComponent();
	~MainComponent() override;

	//==============================================================================
	void prepareToPlay(int/* samplesPerBlockExpected*/, double/* sampleRate*/) override
	{
		// Preparazione per la riproduzione
	}
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
	void releaseResources() override;

	//==============================================================================
	void paint(juce::Graphics& g) override;
	void resized() override;

	inline void showConnectionErrorMessage(const juce::String& messageText)
	{
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
			"Connection error",
			messageText,
			"OK");
	};

private:

	juce::OSCSender Sender;
	juce::Slider volumeSlider;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

