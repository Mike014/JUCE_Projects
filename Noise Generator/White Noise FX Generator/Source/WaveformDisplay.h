#pragma once

#include <JuceHeader.h>

class WaveformDisplay : public juce::Component
{
public:
    WaveformDisplay();
    void updateWaveform(const juce::AudioBuffer<float>& buffer);
    void resetWaveform();
    void paint(juce::Graphics& g) override;
    void setZoomFactor(float zoomFactor);
    void setPanPosition(float panPosition);
    void setPlaybackPosition(float playbackPosition);
    void updateRealTimeWaveform(const juce::AudioBuffer<float>& buffer, int startSample, int numSamples);

private:
    juce::AudioBuffer<float> waveform;
    juce::Path waveformPath;
    float zoomFactor;
    float panPosition;
    float playbackPosition;
    void generateWaveformPath();
    void generateIncrementalWaveformPath(int startSample, int endSample);
    bool isBufferEmpty() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};






