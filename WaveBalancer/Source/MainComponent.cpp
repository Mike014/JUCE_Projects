#include "MainComponent.h"

MainComponent::MainComponent()
{
	formatManager.registerBasicFormats();

	selectFileButton.setButtonText("Select File");
	selectFileButton.addListener(this);
	addAndMakeVisible(selectFileButton);

	conversionBox.addItem("Mono", 1);
	conversionBox.addItem("Stereo", 2);
	addAndMakeVisible(conversionBox);

	convertButton.setButtonText("Convert");
	convertButton.addListener(this);
	addAndMakeVisible(convertButton);

	fileLabel.setText("No file selected", juce::dontSendNotification);
	addAndMakeVisible(fileLabel);

	setSize(150, 175);
}

MainComponent::~MainComponent()
{

}

void MainComponent::resized()
{
	selectFileButton.setBounds(10, 10, 100, 30);
	conversionBox.setBounds(10, 50, 100, 30);
	convertButton.setBounds(10, 90, 100, 30);
	fileLabel.setBounds(10, 130, 100, 30);
}

void MainComponent::buttonClicked(juce::Button* button)
{
	if (button == &selectFileButton)
	{
		auto chooser = std::make_shared<juce::FileChooser>("Select a WAV file to convert", juce::File{}, "*.wav");
		chooser->launchAsync(juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::openMode,
			[this, chooser](const juce::FileChooser& fc)
			{
				auto file = fc.getResult();
				if (file.exists())
				{
					selectedFile = file;
					fileLabel.setText(file.getFileName(), juce::dontSendNotification);

					auto reader = formatManager.createReaderFor(file);
					if (reader != nullptr)
					{
						auto numChannels = reader->numChannels;
						if (numChannels == 1)
						{
							fileLabel.setText("Mono", juce::dontSendNotification);
						}
						else if (numChannels == 2)
						{
							fileLabel.setText("Stereo", juce::dontSendNotification);
						}
					}
				}
			});
	}
	else if (button == &convertButton)
	{
		convertFile(&conversionBox);
	}
}

void MainComponent::convertFile(juce::ComboBox* comboBox)
{
	if (!selectedFile.existsAsFile()) 
	{
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
			"Conversion Error",
			"Please select a file before converting.");
		return;
	}

	int conversionChoice = comboBox->getSelectedId(); 

	std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(selectedFile));
	if (reader == nullptr)
	{
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
			"Conversion Error",
			"Could not read the selected file.");
		return;
	}

	juce::AudioSampleBuffer buffer((int)reader->numChannels, (int)reader->lengthInSamples);
	reader->read(&buffer, 0, (int)reader->lengthInSamples, 0, true, true);

	juce::AudioSampleBuffer convertedBuffer; 

	if (conversionChoice == 1 && reader->numChannels == 2) 
	{
		convertedBuffer.setSize(1, buffer.getNumSamples());
		for (int i = 0; i < buffer.getNumSamples(); ++i)
		{
			const float monoSample = (buffer.getSample(0, i) + buffer.getSample(1, i)) / 2.0f;
			convertedBuffer.setSample(0, i, monoSample);
		}
	}
	else if (conversionChoice == 2 && reader->numChannels == 1) 
	{
		convertedBuffer.setSize(2, buffer.getNumSamples());
		for (int i = 0; i < buffer.getNumSamples(); ++i)
		{
			const float monoSample = buffer.getSample(0, i);
			convertedBuffer.setSample(0, i, monoSample);
			convertedBuffer.setSample(1, i, monoSample);
		}
	}
	else
	{
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
			"Conversion Info",
			"The file is already in the desired format.");
		return;
	}

	juce::File outputFile = selectedFile.getSiblingFile(selectedFile.getFileNameWithoutExtension() + "_converted.wav");
	if (outputFile.existsAsFile())
		outputFile.deleteFile(); 

	std::unique_ptr<juce::FileOutputStream> fileStream(outputFile.createOutputStream());
	if (fileStream == nullptr)
	{
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
			"Conversion Error",
			"Failed to create output file.");
		return;
	}

	if (auto writer = formatManager.findFormatForFileExtension("wav")->createWriterFor(fileStream.get(), reader->sampleRate, convertedBuffer.getNumChannels(), 16, {}, 0))
	{
		if (writer->writeFromAudioSampleBuffer(convertedBuffer, 0, convertedBuffer.getNumSamples()))
		{
			juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
				"Conversion Success",
				"File successfully converted.");
		}
		else
		{
			juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
				"Conversion Error",
				"Failed to write audio data to file.");
		}
		delete writer; 
	}
	else
	{
		juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
			"Conversion Error",
			"Failed to create audio writer.");
	}

	fileStream.release();
}
























