#include "WaveformDisplay.h"

WaveformDisplay::WaveformDisplay()
    : zoomFactor(1.0f), panPosition(0.0f), playbackPosition(0.0f)
{
    setSize(400, 200);
}

void WaveformDisplay::updateWaveform(const juce::AudioBuffer<float>& buffer)
{
    waveform.makeCopyOf(buffer);
    generateWaveformPath();
    repaint();
}

void WaveformDisplay::resetWaveform()
{
    waveform.clear();
    waveformPath.clear();
    repaint();
}

void WaveformDisplay::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    if (waveform.getNumSamples() == 0 || waveform.getNumChannels() == 0)
    {
        g.setColour(juce::Colours::white);
        g.drawText("No waveform data", getLocalBounds(), juce::Justification::centred);
        return;
    }

    // Disegna la forma d'onda
    g.setColour(juce::Colours::white);
    g.strokePath(waveformPath, juce::PathStrokeType(2.0f));

    // Disegna il cursore di riproduzione
    g.setColour(juce::Colours::red);
    auto cursorX = playbackPosition * getWidth();
    g.drawLine(static_cast<float>(cursorX), 0.0f, static_cast<float>(cursorX), static_cast<float>(getHeight()), 2.0f);
}

void WaveformDisplay::generateWaveformPath()
{
    waveformPath.clear();
    auto numSamples = waveform.getNumSamples();
    auto numChannels = waveform.getNumChannels();
    auto width = getWidth();
    auto height = getHeight();

    if (numSamples == 0 || numChannels == 0 || width == 0 || height == 0)
        return;

    auto startSample = static_cast<int>(panPosition * numSamples);
    auto endSample = juce::jmin(startSample + static_cast<int>(numSamples / zoomFactor), numSamples);

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* samples = waveform.getReadPointer(channel);
        if (samples == nullptr)
            continue;

        for (int i = startSample; i < endSample; i += static_cast<int>(1.0f / zoomFactor))
        {
            if (i >= numSamples) break; // Verifica della validità degli indici
            auto x = juce::jmap(static_cast<float>(i - startSample), 0.0f, static_cast<float>(endSample - startSample), 0.0f, static_cast<float>(width));
            auto y = juce::jmap(samples[i], -1.0f, 1.0f, static_cast<float>(height), 0.0f);
            if (i == startSample)
                waveformPath.startNewSubPath(x, y);
            else
                waveformPath.lineTo(x, y);
        }
    }
}

void WaveformDisplay::setZoomFactor(float newZoomFactor)
{
    zoomFactor = newZoomFactor;
    generateWaveformPath();
    repaint();
}

void WaveformDisplay::setPanPosition(float newPanPosition)
{
    panPosition = newPanPosition;
    generateWaveformPath();
    repaint();
}

void WaveformDisplay::setPlaybackPosition(float newPlaybackPosition)
{
    playbackPosition = newPlaybackPosition;
    repaint();
}

void WaveformDisplay::updateRealTimeWaveform(const juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    if (buffer.getNumSamples() == 0 || buffer.getNumChannels() == 0)
        return;

    waveform.makeCopyOf(buffer);
    generateIncrementalWaveformPath(startSample, startSample + numSamples);
    repaint();
}

void WaveformDisplay::generateIncrementalWaveformPath(int startSample, int endSample)
{
    auto numChannels = waveform.getNumChannels();
    auto width = getWidth();
    auto height = getHeight();

    if (numChannels == 0 || width == 0 || height == 0)
        return;

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* samples = waveform.getReadPointer(channel);
        if (samples == nullptr)
            continue;

        for (int i = startSample; i < endSample; i += static_cast<int>(1.0f / zoomFactor))
        {
            if (i >= waveform.getNumSamples()) break; // Verifica della validità degli indici
            auto x = juce::jmap(static_cast<float>(i), 0.0f, static_cast<float>(waveform.getNumSamples()), 0.0f, static_cast<float>(width));
            auto y = juce::jmap(samples[i], -1.0f, 1.0f, static_cast<float>(height), 0.0f);
            if (i == startSample)
                waveformPath.startNewSubPath(x, y);
            else
                waveformPath.lineTo(x, y);
        }
    }
}

bool WaveformDisplay::isBufferEmpty() const
{
    return waveform.getNumSamples() == 0 || waveform.getNumChannels() == 0;
}










