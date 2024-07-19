#include "MainComponent.h"

MainComponent::MainComponent()
{
	setSize(400, 200);

	button.setButtonText("Open File");
	button.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
	
	button.addListener(this);
	addAndMakeVisible(button);

	sampleRateLabel.setText("Sample Rate: ", juce::dontSendNotification);
	sampleRateLabel.setColour(juce::Label::textColourId, juce::Colours::darkblue);
	addAndMakeVisible(sampleRateLabel);

	bitDepthLabel.setText("Bit Depth: ", juce::dontSendNotification);
    bitDepthLabel.setColour(juce::Label::textColourId, juce::Colours::darkblue);
	addAndMakeVisible(bitDepthLabel);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::aquamarine);
}

void MainComponent::resized()
{
	button.setBounds(10, 10, getWidth() - 20, 40);
	sampleRateLabel.setBounds(10, 60, getWidth() - 20, 40);
	bitDepthLabel.setBounds(10, 110, getWidth() - 20, 40);
}

void MainComponent::buttonClicked(juce::Button* clickedButton)
{
    if (clickedButton == &this->button)
    {
        auto chooser = std::make_shared<juce::FileChooser>("Select a WAV file...", juce::File{}, "*.wav");
        chooser->launchAsync(juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::openMode,
            [this, chooser](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.exists())
                {
                    std::unique_ptr<juce::AudioFormatReader> reader(juce::WavAudioFormat().createReaderFor(file.createInputStream().release(), true));

                    if (reader != nullptr)
                    {
                        sampleRateLabel.setText("Sample Rate: " + juce::String(reader->sampleRate), juce::dontSendNotification);
                        bitDepthLabel.setText("Bit Depth: " + juce::String(reader->bitsPerSample), juce::dontSendNotification);
                    }
                }
            });
    }
}



