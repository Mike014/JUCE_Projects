#pragma once

#include <JuceHeader.h>

class WaveformDisplay : public juce::Component
{
public:
   inline void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::lightseagreen); // Cambia il colore di sfondo come preferisci

        g.setColour(juce::Colours::blanchedalmond);
        for (int i = 1; i < audioPoints.size(); ++i)
        {
            float x1 = static_cast<float>(i - 1) / audioPoints.size() * getWidth();
            float y1 = getHeight() / 2.0f * (1.0f - audioPoints[i - 1]);
            float x2 = static_cast<float>(i) / audioPoints.size() * getWidth();
            float y2 = getHeight() / 2.0f * (1.0f - audioPoints[i]);

            g.drawLine(x1, y1, x2, y2);
        }
    }

    inline void setAudioPoints(const juce::Array<float>& points)
    {
        audioPoints = points;
        repaint(); // Questo farà sì che la waveform venga ridisegnata quando i punti audio cambiano
    }

private:
    juce::Array<float> audioPoints; // Questo array memorizza i punti audio per la waveform
};

class MainComponent  : public juce::AudioAppComponent, public juce::ChangeListener, public juce::Slider::Listener
{

    enum TransportState
	{
		Stopped,
		Starting,
		Playing,
        Pausing,
		Stopping
	};

public:
 
    MainComponent();
    ~MainComponent() override;

    inline void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
		transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    }
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    void playButtonClicked();
    void stopButtonClicked();
    void openButtonClicked();
    void pauseButtonClicked();

    void changeState(TransportState newState);
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void sliderValueChanged(juce::Slider* slider) override;
    juce::Label trackLabel;

    WaveformDisplay waveformDisplay;

    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::TextButton pauseButton;
    juce::Slider volumeSlider;

    TransportState state;
    juce::Array<float> audioPoints;

    std::unique_ptr<juce::FileChooser> chooser;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
