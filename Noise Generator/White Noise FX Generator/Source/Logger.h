#pragma once

#include <JuceHeader.h>

class Logger
{
public:
    static void log(const juce::String& message)
    {
        juce::Logger::writeToLog(message);
    }

    static void logException(const std::exception& e)
    {
        juce::Logger::writeToLog("Exception: " + juce::String(e.what()));
    }
};




