#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize (800, 600);

    addAndMakeVisible(&levelSlider);
    levelSlider.setRange(0.0, 1.0);
    levelSlider.setValue(0.7);
    levelSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    levelSlider.addListener(this);

    addAndMakeVisible(&levelLabel);

    addAndMakeVisible(&waveformDisplay);

    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    levelSlider.setBounds(getWidth() - 200, getHeight() - 220, 200, 200);
    levelLabel.setBounds(levelSlider.getX(), levelSlider.getY() - 20, 80, 20);
    waveformDisplay.setBounds(0, 0, getWidth(), getHeight() - 220);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    auto* device = deviceManager.getCurrentAudioDevice();
    auto activeInputChannels = device->getActiveInputChannels();
    auto activeOutputChannels = device->getActiveOutputChannels();
    auto maxInputChannels = activeInputChannels.getHighestBit() + 1;
    auto maxOutputChannels = activeOutputChannels.getHighestBit() + 1;
    auto level = (float)levelSlider.getValue();

    juce::Array<float> points;

    for (auto channel = 0; channel < maxOutputChannels; ++channel)
    {
        if ((!activeOutputChannels[channel]) || maxInputChannels == 0)
        {
            bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
        }
        else
        {
            auto actualInputChannel = channel % maxInputChannels;

            if (!activeInputChannels[channel])
            {
                bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
            }
            else
            {
                auto* inBuffer = bufferToFill.buffer->getReadPointer(actualInputChannel,
                    bufferToFill.startSample);
                auto* outBuffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);

                for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
                {
                    auto noise = (random.nextFloat() * 2.0f) - 1.0f;
                    outBuffer[sample] = inBuffer[sample] + (inBuffer[sample] * noise * level);

                    // Aggiungi il campione al buffer dei punti se stiamo elaborando il primo canale
                    if (channel == 0)
                    {
                        points.add(inBuffer[sample]);
                    }
                }
            }
        }
    }

    // Aggiorna i punti audio in waveformDisplay
    waveformDisplay.setAudioPoints(points);
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &levelSlider)
    {
        levelLabel.setText("Level: " + juce::String(levelSlider.getValue(), 2), juce::dontSendNotification);
    }
}
