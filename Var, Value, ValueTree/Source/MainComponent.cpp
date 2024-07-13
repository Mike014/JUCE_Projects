#include "MainComponent.h"

const juce::Identifier MainComponent::personId("person");
const juce::Identifier MainComponent::nameId("name");
const juce::Identifier MainComponent::ageId("age");

MainComponent::MainComponent()
{
	setSize(600, 400);

	nameLabel.setText("Name: ", juce::dontSendNotification);
	addAndMakeVisible(nameLabel);
	nameEditor.onTextChange = [this]
		{ nameValue.setValue(nameEditor.getText()); };
	nameValue.referTo(personData.getPropertyAsValue(nameId, nullptr));
	addAndMakeVisible(nameEditor);

	ageLabel.setText("Age: ", juce::dontSendNotification);
	addAndMakeVisible(ageLabel);
	ageEditor.onTextChange = [this] {
		ageVar = ageEditor.getText().getIntValue();
		personData.setProperty(ageId, ageVar, nullptr);
		};
	addAndMakeVisible(ageEditor);

	personData.setProperty(nameId, "", nullptr);
	personData.setProperty(ageId, 0, nullptr);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
	int labelWidth = 50;
	int componentHeight = 20;
	int componentSpacing = 10;

	nameLabel.setBounds(10, 10, labelWidth, componentHeight);
	nameEditor.setBounds(nameLabel.getRight() + componentSpacing, 10, getWidth() - nameLabel.getWidth() - 20 - componentSpacing, componentHeight);

	ageLabel.setBounds(10, nameLabel.getBottom() + componentSpacing, labelWidth, componentHeight);
	ageEditor.setBounds(ageLabel.getRight() + componentSpacing, nameLabel.getBottom() + componentSpacing, getWidth() - ageLabel.getWidth() - 20 - componentSpacing, componentHeight);
}





