#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class SineWaveGenerator : public juce::AudioSource
{
public:
    SineWaveGenerator();
    void setFrequency(double frequency, double sampleRate);
    double getCurrentFrequency() const;
    void setVolumeLevel(float level);
    float getVolumeLevel() const;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

private:
    double currentAngle;
    double angleDelta;
    double currentFrequency;
    float volumeLevel;
};

class MainComponent : public juce::AudioAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void paint(juce::Graphics& g) override;
    void mouseMove(const juce::MouseEvent& event) override;

private:
    SineWaveGenerator sineWaveGenerator;
};




