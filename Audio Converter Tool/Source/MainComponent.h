#pragma once

#include <JuceHeader.h>

class MainComponent : public juce::Component, public juce::Button::Listener
{
public:
    MainComponent();
    ~MainComponent() override; 

    void paint(juce::Graphics&) override; 
    void resized() override; 
    void buttonClicked(juce::Button* clickedButton) override; 

private:
    juce::TextButton button; // Button to open file chooser
    juce::TextButton convertButton; // Button to start conversion
    juce::Label sampleRateLabel; // Label to display sample rate
    juce::Label bitDepthLabel; // Label to display bit depth
    juce::ComboBox bitDepthComboBox; // ComboBox to select bit depth
    juce::ComboBox sampleRateComboBox; // ComboBox to select sample rate
    juce::File selectedFile; // Holds the selected file

    void convertFile(); // Method to convert the file

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent) 
};

