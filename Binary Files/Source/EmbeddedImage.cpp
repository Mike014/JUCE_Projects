#include "EmbeddedImage.h"

EmbeddedImage::EmbeddedImage()
{
    // Utilizza il percorso completo dell'immagine .jfif
    juce::File imageFile("C:\\Users\\PC\\Downloads\\1718795699281.jfif");
    juce::FileInputStream inputStream(imageFile);
    if (inputStream.openedOk())
    {
        image = juce::ImageFileFormat::loadFrom(inputStream);
    }
    else
    {
        juce::Logger::writeToLog("Impossibile aprire il file immagine.");
    }
}

void EmbeddedImage::paint(juce::Graphics& g)
{
    if (!image.isNull())
        g.drawImageAt(image, 0, 0);
}





