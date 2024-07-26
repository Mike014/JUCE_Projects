#pragma once

#include <JuceHeader.h>
#include <cstdint> 
#include <algorithm> 
#include "Utils.h"

class MainComponent : public juce::AudioAppComponent,
	public juce::Button::Listener,
	public juce::Slider::Listener,
	public juce::ChangeListener

{
public:
	MainComponent();
	~MainComponent() override;

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
	void releaseResources() override;

	void paint(juce::Graphics& g) override;
	void resized() override;

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;
	void buttonClicked(juce::Button* button) override;
	void sliderValueChanged(juce::Slider* slider) override;

private:
	enum TransportState
	{
		Stopped,
		Starting,
		Playing,
		Stopping
	};

	void changeState(TransportState newState);
	void loadFile(const juce::File& file);
	void saveFile(const juce::File& file);
	void updateThumbnail();
	void applyAdvancedSmoothing(juce::AudioBuffer<float>& buffer, int numSamples);
	void exportFileWithVolume(const juce::File& file);

	// Componenti UI
	juce::TextButton importButton;
	juce::TextButton exportButton;
	juce::TextButton playButton;
	juce::TextButton stopButton;
	juce::Slider volumeSlider;

	// Gestione audio
	juce::AudioFormatManager formatManager;
	std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
	juce::AudioTransportSource transportSource;
	juce::AudioThumbnailCache thumbnailCache;
	juce::AudioThumbnail thumbnail;

	TransportState state;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};









