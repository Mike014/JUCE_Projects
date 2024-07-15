#ifndef __MAINCOMPONENT_H__
#define __MAINCOMPONENT_H__

#include "JuceHeader.h"

class MainComponent : public juce::Component, public juce::Button::Listener, public juce::Thread
{
public: 
	MainComponent();
	~MainComponent();

	void resized() override;
	void buttonClicked(juce::Button* button) override;
	void run() override;

private:
	juce::TextButton startThreadButton;
	juce::TextButton stopThreadButton;
	juce::Label counterLabel; 
	juce::Atomic<int> counter{ 0 };

	void updateCounterLabel();
};

























#endif // __MAINCOMPONENT_H__







