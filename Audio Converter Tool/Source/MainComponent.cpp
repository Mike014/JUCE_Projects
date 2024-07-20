#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(400, 300); // Set the size of the main component

    // Configure the open file button
    button.setButtonText("Open File");
    button.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
    button.addListener(this);
    addAndMakeVisible(button);

    // Configure the convert button
    convertButton.setButtonText("Convert to ...");
    convertButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
    convertButton.addListener(this);
    addAndMakeVisible(convertButton);

    // Configure the sample rate label
    sampleRateLabel.setText("Sample Rate: ", juce::dontSendNotification);
    sampleRateLabel.setColour(juce::Label::textColourId, juce::Colours::darkblue);
    addAndMakeVisible(sampleRateLabel);

    // Configure the bit depth label
    bitDepthLabel.setText("Bit Depth: ", juce::dontSendNotification);
    bitDepthLabel.setColour(juce::Label::textColourId, juce::Colours::darkblue);
    addAndMakeVisible(bitDepthLabel);

    // Add items to the sample rate ComboBox
    sampleRateComboBox.addItem("44100", 1);
    sampleRateComboBox.addItem("48000", 2);
    addAndMakeVisible(sampleRateComboBox);

    // Add items to the bit depth ComboBox
    bitDepthComboBox.addItem("16", 1);
    bitDepthComboBox.addItem("24", 2);
    addAndMakeVisible(bitDepthComboBox);
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
    // Set the bounds of the components
    button.setBounds(10, 10, getWidth() - 20, 40);
    convertButton.setBounds(10, 260, getWidth() - 20, 40);
    sampleRateLabel.setBounds(10, 60, getWidth() - 20, 40);
    bitDepthLabel.setBounds(10, 110, getWidth() - 20, 40);
    sampleRateComboBox.setBounds(10, 160, getWidth() - 20, 40);
    bitDepthComboBox.setBounds(10, 210, getWidth() - 20, 40);
}

void MainComponent::buttonClicked(juce::Button* clickedButton)
{
    if (clickedButton == &this->button)
    {
        // Open a file chooser dialog
        auto chooser = std::make_shared<juce::FileChooser>("Select a WAV file...", juce::File{}, "*.wav");
        chooser->launchAsync(juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::openMode,
            [this, chooser](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.exists())
                {
                    // Read the selected file
                    std::unique_ptr<juce::AudioFormatReader> reader(juce::WavAudioFormat().createReaderFor(file.createInputStream().release(), true));
                    selectedFile = file;

                    if (reader != nullptr)
                    {
                        // Display the sample rate and bit depth of the selected file
                        sampleRateLabel.setText("Sample Rate: " + juce::String(reader->sampleRate), juce::dontSendNotification);
                        bitDepthLabel.setText("Bit Depth: " + juce::String(reader->bitsPerSample), juce::dontSendNotification);
                    }
                }
            });
    }
    else if (clickedButton == &convertButton)
    {
        convertFile(); 
    }
}

void MainComponent::convertFile() {
    if (!selectedFile.existsAsFile()) {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Error", "No file selected", "OK");
        return;
    }

    int sampleRateIndex = sampleRateComboBox.getSelectedId();
    int bitDepthIndex = bitDepthComboBox.getSelectedId();

    if (sampleRateIndex == 0 || bitDepthIndex == 0) 
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Error", "Please select both sample rate and bit depth", "OK");
        return;
    }

    auto sampleRate = sampleRateIndex == 1 ? 44100 : 48000;
    auto bitDepth = bitDepthIndex == 1 ? 16 : 24;

    // Read the file
    std::unique_ptr<juce::AudioFormatReader> reader(juce::WavAudioFormat().createReaderFor(selectedFile.createInputStream().release(), true));
    if (reader == nullptr)
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Error", "Unable to read the file", "OK");
        return;
    }

    // Modify the file name to include "_newfile"
    auto newFileName = selectedFile.getFileNameWithoutExtension() + "_newfile.wav";
    auto newFile = selectedFile.getSiblingFile(newFileName);

    // Write the converted file
    std::unique_ptr<juce::AudioFormatWriter> writer(juce::WavAudioFormat().createWriterFor(newFile.createOutputStream().release(), sampleRate, reader->numChannels, bitDepth, {}, 0));
    if (writer == nullptr)
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Error", "Unable to write the file", "OK");
        return;
    }

    // Use std::unique_ptr for automatic memory management
    std::unique_ptr<juce::AudioSampleBuffer> buffer(std::make_unique<juce::AudioSampleBuffer>(reader->numChannels, reader->lengthInSamples));
    reader->read(buffer.get(), 0, reader->lengthInSamples, 0, true, true);

    writer->writeFromAudioSampleBuffer(*buffer, 0, reader->lengthInSamples);

    // Show a message with the location of the converted file
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon, "Conversion", "File successfully converted to: " + newFile.getFullPathName(), "OK");
}


