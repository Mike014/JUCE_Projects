#include "MainComponent.h"

MainComponent::MainComponent()
    : juce::Thread("Example Thread")
{
    addAndMakeVisible(&startThreadButton);
    startThreadButton.addListener(this);
    startThreadButton.setButtonText("Start Thread");

    addAndMakeVisible(&stopThreadButton);
    stopThreadButton.addListener(this);
    stopThreadButton.setButtonText("Stop Thread");

    addAndMakeVisible(&counterLabel);
    counterLabel.setText("Counter: 0", juce::dontSendNotification);

    setSize(400, 200);
}

MainComponent::~MainComponent()
{
    stopThread(2000);
}

void MainComponent::resized()
{
    startThreadButton.setBounds(10, 10, getWidth() - 20, 30);
    stopThreadButton.setBounds(10, 50, getWidth() - 20, 30);
    counterLabel.setBounds(10, 90, getWidth() - 20, 30);
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &startThreadButton)
    {
        startThread();
    }
    else if (button == &stopThreadButton)
    {
        stopThread(2000);
    }
}

void MainComponent::run()
{
    while (!threadShouldExit())
    {
        counter += 1;
        juce::MessageManager::callAsync([this]() { updateCounterLabel(); });
        juce::Thread::sleep(500);
    }
}

void MainComponent::updateCounterLabel()
{
    counterLabel.setText("Counter: " + juce::String(counter.get()), juce::dontSendNotification);
}


