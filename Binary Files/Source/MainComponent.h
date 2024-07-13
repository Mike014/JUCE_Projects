#pragma once

#include "JuceHeader.h"
// Include the directory of the header file of the EmbeddedImage class
#include "EmbeddedImage.h"

class MainComponent : public juce::Component
{
public:
	MainComponent();
	void resized() override;

private:
	EmbeddedImage embeddedImage;
};



