#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
	: state(Stopped)
{
	// Make sure you set the size of the component after
	// you add any child components.
	setSize(400, 400);// Set the size of the window height and width

	addAndMakeVisible(&openButton);
	openButton.setButtonText("Open...");
	openButton.onClick = [this] { openButtonClicked(); };

	addAndMakeVisible(&playButton);
	playButton.setButtonText("Play");
	playButton.onClick = [this] { playButtonClicked(); };
	playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::cyan);
	playButton.setEnabled(false);

	addAndMakeVisible(&stopButton);
	stopButton.setButtonText("Stop");
	stopButton.onClick = [this] { stopButtonClicked(); };
	stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
	stopButton.setEnabled(false);


	addAndMakeVisible(&pauseButton);
	pauseButton.setButtonText("Pause");
	pauseButton.onClick = [this] { pauseButtonClicked(); };
	pauseButton.setColour(juce::TextButton::buttonColourId, juce::Colours::burlywood);
	pauseButton.setEnabled(false);

	addAndMakeVisible(&volumeSlider);
	volumeSlider.onValueChange = [this] { transportSource.setGain(volumeSlider.getValue()); };
	volumeSlider.setRange(0.0, 1.0);
	volumeSlider.setValue(0.75);
	volumeSlider.setSize(175, 30);
	volumeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

	addAndMakeVisible(waveformDisplay);

	addAndMakeVisible(&trackLabel);
	trackLabel.setText("No track loaded", juce::dontSendNotification);

	formatManager.registerBasicFormats();
	transportSource.addChangeListener(this);

	if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
		&& !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
	{
		juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
			[&](bool granted) { setAudioChannels(granted ? 2 : 0, 2); });
	}
	else
	{
		setAudioChannels(0, 2);
	}
}

//==============================================================================

MainComponent::~MainComponent()
{
	// This shuts down the audio device and clears the audio source.
	shutdownAudio();
}

//==============================================================================

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	if (readerSource.get() == nullptr)
	{
		bufferToFill.clearActiveBufferRegion();
		return;
	}

	transportSource.getNextAudioBlock(bufferToFill);

	juce::Array<float> audioPoints;
	auto* channelData = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);
	for (int i = 0; i < bufferToFill.numSamples; ++i)
	{
		audioPoints.add(*channelData++);
	}

	waveformDisplay.setAudioPoints(audioPoints);
}

//==============================================================================

void MainComponent::releaseResources()
{
	transportSource.releaseResources();
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

//==============================================================================

void MainComponent::resized()
{
	openButton.setBounds(10, 10, getWidth() - 20, 20);
	playButton.setBounds(10, 40, getWidth() - 20, 20);
	stopButton.setBounds(10, 70, getWidth() - 20, 20);
	pauseButton.setBounds(10, 100, getWidth() - 20, 20);
	volumeSlider.setBounds(10, 130, getWidth() - 20, 20);
	waveformDisplay.setBounds(10, 160, getWidth() - 20, getHeight() - 170);
	trackLabel.setBounds(10, 160, getWidth() - 20, 20);
}

//==============================================================================

void MainComponent::changeState(TransportState newState)
{
	if (state != newState)
	{
		state = newState;

		switch (state)
		{
		case Stopped:
			stopButton.setEnabled(false);
			playButton.setEnabled(true);
			pauseButton.setEnabled(false);
			transportSource.setPosition(0.0);
			break;

		case Starting:
			playButton.setEnabled(false);
			pauseButton.setEnabled(true);
			stopButton.setEnabled(true);
			transportSource.start();
			break;

		case Playing:
			stopButton.setEnabled(true);
			playButton.setEnabled(false);
			pauseButton.setEnabled(true);
			break;

		case Pausing:
			stopButton.setEnabled(true);
			playButton.setEnabled(true);
			pauseButton.setEnabled(false);
			break;

		case Stopping:
			transportSource.stop();
			break;
		}
	}
}

//==============================================================================

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	if (source == &transportSource)
	{
		if (transportSource.isPlaying())
			changeState(Playing);
		else if (state == Stopping)
			changeState(Stopped);
		/*else if (state == Pausing)
			changeState(Pausing);*/
	}
}

//==============================================================================

void MainComponent::playButtonClicked()
{
	changeState(Starting);
}

//==============================================================================

void MainComponent::stopButtonClicked()
{
	changeState(Stopping);
}

//==============================================================================

void MainComponent::openButtonClicked()
{
	chooser = std::make_unique<juce::FileChooser>("Select a Wave file to play...",
		juce::File{},
		"*.wav");
	auto chooserFlags = juce::FileBrowserComponent::openMode
		| juce::FileBrowserComponent::canSelectFiles;

	chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
		{
			auto file = fc.getResult();

			if (file != juce::File{})
			{
				auto* reader = formatManager.createReaderFor(file);

				if (reader != nullptr)
				{
					auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
					transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
					playButton.setEnabled(true);
					readerSource.reset(newSource.release());
				}
				trackLabel.setText("Now playing: " + file.getFileName(), juce::dontSendNotification);
			}
		});
}

//==============================================================================

void MainComponent::pauseButtonClicked()
{
	if (state == Playing)
	{
		changeState(Pausing);
		transportSource.stop();
	}
	else if (state == Pausing)
	{
		changeState(Playing);
		transportSource.start();
	}
}

//==============================================================================

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
	if (slider == &volumeSlider)
	{
		transportSource.setGain(volumeSlider.getValue());
	}
}

//==============================================================================

