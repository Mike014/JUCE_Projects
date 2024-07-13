#pragma once

#include "JuceHeader.h"

class EmbeddedImage : public juce::Component 
{
public:
    EmbeddedImage();
    void paint(juce::Graphics&) override;

private:
    juce::Image image;
};

