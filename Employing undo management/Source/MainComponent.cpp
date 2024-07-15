#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(600, 400);

    addAndMakeVisible(undoButton);
    undoButton.onClick = [this] { undoManager.undo(); };

    addAndMakeVisible(dataEditor);
    addAndMakeVisible(updateButton);
    updateButton.onClick = [this] { updateData(); };

    personData.addListener(this);

    addAndMakeVisible(dataDisplay);
    dataDisplay.setJustificationType(juce::Justification::centredLeft);
}

MainComponent::~MainComponent()
{
    personData.removeListener(this);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    undoButton.setBounds(10, 10, getWidth() - 20, 40);
    dataEditor.setBounds(10, 60, getWidth() - 20, 40);
    updateButton.setBounds(10, 110, getWidth() - 20, 40);
    dataDisplay.setBounds(10, 160, getWidth() - 20, 40);
}

void MainComponent::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    auto newValue = treeWhosePropertyHasChanged.getProperty(property).toString();
    DBG("Change Property " << property.toString() << ", New Value: " << newValue);
    dataDisplay.setText("ValueTree Data: " + newValue, juce::dontSendNotification); 
}

void MainComponent::updateData()
{
    auto text = dataEditor.getText();
    if (text.isNotEmpty()) {
        undoManager.beginNewTransaction();
        personData.setProperty("Data", text, &undoManager);
    }
    else {
        DBG("No text entered, no update performed");
    }
}






