#pragma once

#include <JuceHeader.h>

class MainComponent : public juce::AudioAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::TextButton openButton;
    juce::TextButton clearButton;

    juce::AudioFormatManager formatManager;
    juce::AudioSampleBuffer fileBuffer;
    int position = 0;

    std::unique_ptr<juce::FileChooser> chooser;

    void openButtonClicked();
    void clearButtonClicked();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
