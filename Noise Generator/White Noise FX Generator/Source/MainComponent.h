#pragma once

#include <JuceHeader.h>
#include "WaveformDisplay.h"
#include <thread>
#include <atomic>

enum class NoiseType
{
    None,
    White,
    Pink
};

class MainComponent : public juce::AudioAppComponent, public juce::Timer
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
    void playButtonClicked();
    void stopButtonClicked();
    void whiteNoiseButtonClicked();
    void pinkNoiseButtonClicked();
    void exportButtonClicked();
    void timerCallback() override;
    void stopNoiseThread();
    void generateNoise();

    juce::TextButton playButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton whiteNoiseButton{ "White Noise" };
    juce::TextButton pinkNoiseButton{ "Pink Noise" };
    juce::Slider volumeSlider;
    juce::Label volumeLabel;
    juce::Slider durationSlider;
    juce::Label durationLabel;
    juce::TextButton exportButton{ "Export" };
	juce::Random random;
    WaveformDisplay waveformDisplay;

    std::atomic<bool> isPlaying;
    std::atomic<int> playbackDurationMs;
    std::atomic<int> remainingPlaybackTimeMs;
    double sampleRate;
    NoiseType noiseType;

    std::thread noiseThread;
    std::atomic<bool> stopThread{ false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};





