#include "MainComponent.h"

SineWaveGenerator::SineWaveGenerator() : currentAngle(0), angleDelta(0), currentFrequency(1000.0), volumeLevel(1.0f) {}

void SineWaveGenerator::setFrequency(double frequency, double sampleRate)
{
    currentFrequency = frequency;
    auto cyclesPerSample = frequency / sampleRate;
    angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
}

double SineWaveGenerator::getCurrentFrequency() const
{
    return currentFrequency;
}

void SineWaveGenerator::setVolumeLevel(float level)
{
    volumeLevel = level;
}

float SineWaveGenerator::getVolumeLevel() const
{
    return volumeLevel;
}

void SineWaveGenerator::prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate)
{
    setFrequency(currentFrequency, sampleRate);
}

void SineWaveGenerator::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    auto* leftChannel = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    auto* rightChannel = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

    for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
    {
        auto value = (float)std::sin(currentAngle) * volumeLevel;
        currentAngle += angleDelta;

        leftChannel[sample] = value;
        rightChannel[sample] = value;
    }
}

void SineWaveGenerator::releaseResources() {}

MainComponent::MainComponent()
{
    setSize(800, 600);
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    sineWaveGenerator.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    sineWaveGenerator.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    sineWaveGenerator.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    auto text = "Sine Wave Frequency: " + juce::String(sineWaveGenerator.getCurrentFrequency()) + " Hz, " +
        "Volume: " + juce::String(sineWaveGenerator.getVolumeLevel());
    g.drawText(text, getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::mouseMove(const juce::MouseEvent& event)
{
    auto height = getHeight();
    auto mouseY = event.getPosition().getY();
    auto volume = 1.0f - (mouseY / static_cast<float>(height));
    volume = std::clamp(volume, 0.0f, 1.0f);
    sineWaveGenerator.setVolumeLevel(volume);
    repaint();
}







