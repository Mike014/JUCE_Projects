#pragma once

#include <JuceHeader.h>

class MainComponent : public juce::Component
{
public:
	MainComponent();
	~MainComponent() override;

	void paint(juce::Graphics&) override;
	void resized() override;

private:
	juce::Label nameLabel;
	juce::TextEditor nameEditor;
	juce::Value nameValue;

	juce::Label ageLabel;
	juce::TextEditor ageEditor;
	juce::var ageVar;

	juce::ValueTree personData;

	static const juce::Identifier personId;
	static const juce::Identifier nameId;
	static const juce::Identifier ageId;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
