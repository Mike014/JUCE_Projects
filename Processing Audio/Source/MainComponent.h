#pragma once

#include <JuceHeader.h>

class WaveformDisplay : public juce::Component
{
public:
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::lightseagreen);

        g.setColour(juce::Colours::blanchedalmond);
        if (audioPoints.size() > 1)
        {
            float scale = static_cast<float>(getWidth()) / static_cast<float>(audioPoints.size() - 1);
            for (int i = 1; i < audioPoints.size(); ++i)
            {
                g.drawLine((i - 1) * scale, getHeight() / 2.0f * (1.0f - audioPoints[i - 1]),
                    i * scale, getHeight() / 2.0f * (1.0f - audioPoints[i]));
            }
        }
    }

    void setAudioPoints(const juce::Array<float>& points)
    {
        audioPoints = points;
        repaint();
    }

private:
    juce::Array<float> audioPoints;
};

class MainComponent : public juce::AudioAppComponent, public juce::Slider::Listener
{
public:
 
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int/* samplesPerBlockExpected*/, double/* sampleRate*/) override
    {
        // Preparazione per la riproduzione
    }
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void sliderValueChanged(juce::Slider* slider);
    void releaseResources() override
    {
        // Rilascio delle risorse
    }

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    
    juce::Random random;
    juce::Slider levelSlider;
    juce::Label levelLabel;
    WaveformDisplay waveformDisplay;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
