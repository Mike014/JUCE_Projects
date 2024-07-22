#pragma once

#include <JuceHeader.h>

class MainComponent : public juce::Component, public juce::Button::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    inline void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::aquamarine);
        fileLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    };

    void resized() override;

    void buttonClicked(juce::Button* button) override;

private:

    juce::TextButton selectFileButton;
    juce::ComboBox conversionBox;
    juce::TextButton convertButton;
    juce::File selectedFile;
    juce::Label fileLabel;
    juce::AudioFormatManager formatManager;

    void convertFile(juce::ComboBox* comboBox);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)

};


