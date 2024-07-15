#pragma once

#include <JuceHeader.h>

class MainComponent : public juce::Component, public juce::ValueTree::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

private:
    juce::UndoManager undoManager;
    juce::TextButton undoButton{ "Undo" };
    juce::TextButton updateButton{ "Update" };
    juce::TextEditor dataEditor;
    juce::ValueTree personData{ "Person" };
    juce::Label dataDisplay;

    void updateData();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};



