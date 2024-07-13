#include "MainComponent.h"

MainComponent::MainComponent()
{
	addAndMakeVisible(embeddedImage);

	setSize(600, 400);
}

void MainComponent::resized()
{
	embeddedImage.setBounds(0, 0, getWidth(), getHeight());
}
