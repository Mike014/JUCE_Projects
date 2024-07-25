#include "MainComponent.h"
#include "Logger.h"

MainComponent::MainComponent()
    : isPlaying(false), playbackDurationMs(0), remainingPlaybackTimeMs(0), sampleRate(0.0), noiseType(NoiseType::None)
{
    Logger::log("MainComponent: Costruttore iniziato");

    setSize(800, 600);
    setAudioChannels(2, 2);

    addAndMakeVisible(playButton);
    playButton.onClick = [this] { playButtonClicked(); };

    addAndMakeVisible(stopButton);
    stopButton.onClick = [this] { stopButtonClicked(); };

    addAndMakeVisible(whiteNoiseButton);
    whiteNoiseButton.onClick = [this] { whiteNoiseButtonClicked(); };

    addAndMakeVisible(pinkNoiseButton);
    pinkNoiseButton.onClick = [this] { pinkNoiseButtonClicked(); };

    addAndMakeVisible(volumeSlider);
    volumeSlider.setRange(0.0, 1.0);
    volumeSlider.setValue(0.5);

    addAndMakeVisible(volumeLabel);
    volumeLabel.setText("Volume", juce::dontSendNotification);

    addAndMakeVisible(durationSlider);
    durationSlider.setRange(0.0, 25.0);
    durationSlider.setValue(2.0);
    durationSlider.setTextValueSuffix(" s");

    addAndMakeVisible(durationLabel);
    durationLabel.setText("Duration", juce::dontSendNotification);

    addAndMakeVisible(exportButton);
    exportButton.onClick = [this] { exportButtonClicked(); };

    addAndMakeVisible(waveformDisplay);

    Logger::log("MainComponent: Costruttore completato");
}

MainComponent::~MainComponent()
{
    stopThread = true;
    if (noiseThread.joinable())
        noiseThread.join();
    shutdownAudio();
    stopNoiseThread();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double newSampleRate)
{
    sampleRate = newSampleRate;
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    Logger::log("getNextAudioBlock: Inizio");
    if (isPlaying)
    {
        auto currentVolume = static_cast<float>(volumeSlider.getValue());
        Logger::log("Volume attuale: " + std::to_string(currentVolume));

        auto numSamples = bufferToFill.numSamples;

        for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
        {
            auto* buffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);

            for (auto sample = 0; sample < numSamples; ++sample)
            {
                buffer[sample] = (random.nextFloat() * 2.0f - 1.0f) * currentVolume;
            }
        }
    }
    else
    {
        bufferToFill.clearActiveBufferRegion();
    }
    Logger::log("getNextAudioBlock: Fine");
}

void MainComponent::releaseResources()
{
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    auto buttonHeight = 30;
    auto sliderHeight = 20;

    playButton.setBounds(bounds.removeFromTop(buttonHeight));
    stopButton.setBounds(bounds.removeFromTop(buttonHeight));
    whiteNoiseButton.setBounds(bounds.removeFromTop(buttonHeight));
    pinkNoiseButton.setBounds(bounds.removeFromTop(buttonHeight));

    auto volumeBounds = bounds.removeFromTop(sliderHeight);
    volumeSlider.setBounds(volumeBounds.removeFromLeft(volumeBounds.getWidth() - 50));
    volumeLabel.setBounds(volumeBounds);

    auto durationBounds = bounds.removeFromTop(sliderHeight);
    durationSlider.setBounds(durationBounds.removeFromLeft(durationBounds.getWidth() - 50));
    durationLabel.setBounds(durationBounds);

    exportButton.setBounds(bounds.removeFromTop(buttonHeight));

    waveformDisplay.setBounds(bounds);
}

void MainComponent::playButtonClicked()
{
    isPlaying = true;
    playButton.setEnabled(false);
    stopButton.setEnabled(true);

    playbackDurationMs.store(static_cast<int>(durationSlider.getValue() * 1000));
    remainingPlaybackTimeMs.store(playbackDurationMs.load());

    if (sampleRate > 0)
    {
        stopThread = false;
        noiseThread = std::thread(&MainComponent::generateNoise, this);
    }

    startTimer(50); // Aumenta l'intervallo del timer per migliorare le performance
}

void MainComponent::stopButtonClicked()
{
    isPlaying = false;
    playButton.setEnabled(true);
    stopButton.setEnabled(false);
    stopTimer();
    remainingPlaybackTimeMs.store(0);
    waveformDisplay.resetWaveform();

    stopThread = true;
    if (noiseThread.joinable())
        noiseThread.join();
}

void MainComponent::whiteNoiseButtonClicked()
{
    Logger::log("whiteNoiseButtonClicked: Inizio");
    noiseType = NoiseType::White;
    Logger::log("whiteNoiseButtonClicked: Fine");
}

void MainComponent::pinkNoiseButtonClicked()
{
    Logger::log("pinkNoiseButtonClicked: Inizio");
    noiseType = NoiseType::Pink;
    Logger::log("pinkNoiseButtonClicked: Fine");
}

void MainComponent::exportButtonClicked()
{
    // Implementazione per l'esportazione (attualmente vuota)
}

void MainComponent::timerCallback()
{
    Logger::log("timerCallback: Inizio");
    if (remainingPlaybackTimeMs.load() > 0)
    {
        remainingPlaybackTimeMs.fetch_sub(50); // Aggiorna ogni 50 millisecondi
        float playbackPosition = 1.0f - (remainingPlaybackTimeMs.load() / static_cast<float>(playbackDurationMs.load()));
        waveformDisplay.setPlaybackPosition(playbackPosition);
    }
    else
    {
        stopButtonClicked();
    }
    Logger::log("timerCallback: Fine");
}

void MainComponent::generateNoise()
{
    int numSamples = static_cast<int>(sampleRate * durationSlider.getValue());
    juce::AudioBuffer<float> buffer(2, numSamples);

    if (noiseType == NoiseType::White)
    {
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* samples = buffer.getWritePointer(channel);
            for (int i = 0; i < numSamples; ++i)
            {
                if (stopThread) return;
                samples[i] = random.nextFloat() * 2.0f - 1.0f; // Genera rumore bianco
            }
        }
    }
    else if (noiseType == NoiseType::Pink)
    {
        float b0 = 0.0f, b1 = 0.0f, b2 = 0.0f, b3 = 0.0f, b4 = 0.0f, b5 = 0.0f, b6 = 0.0f;

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* samples = buffer.getWritePointer(channel);
            for (int i = 0; i < numSamples; ++i)
            {
                if (stopThread) return;
                float white = random.nextFloat() * 2.0f - 1.0f;
                b0 = 0.99886f * b0 + white * 0.0555179f;
                b1 = 0.99332f * b1 + white * 0.0750759f;
                b2 = 0.96900f * b2 + white * 0.1538520f;
                b3 = 0.86650f * b3 + white * 0.3104856f;
                b4 = 0.55000f * b4 + white * 0.5329522f;
                b5 = -0.7616f * b5 - white * 0.0168980f;
                samples[i] = b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362f;
                samples[i] *= 0.11f; // (roughly) compensate for gain
                b6 = white * 0.115926f;
            }
        }
    }

    waveformDisplay.updateWaveform(buffer);
}

void MainComponent::stopNoiseThread()
{
    stopThread = true;
    if (noiseThread.joinable())
        noiseThread.join();
}



	





