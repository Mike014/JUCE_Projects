#include "MainComponent.h"

MainComponent::MainComponent()
    : thumbnailCache(5), thumbnail(32, formatManager, thumbnailCache), state(Stopped) // Aumenta il numero di campioni per pixel
{
    // Inizializzazione componenti UI
    addAndMakeVisible(importButton);
    importButton.setButtonText("Importa");
    importButton.addListener(this);

    addAndMakeVisible(exportButton);
    exportButton.setButtonText("Esporta");
    exportButton.addListener(this);

    addAndMakeVisible(playButton);
    playButton.setButtonText("Play");
    playButton.addListener(this);
    playButton.setEnabled(true);

    addAndMakeVisible(stopButton);
    stopButton.setButtonText("Stop");
    stopButton.addListener(this);
    stopButton.setEnabled(false);

    addAndMakeVisible(volumeSlider);
    volumeSlider.setRange(0.0, 1.0);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);

    formatManager.registerBasicFormats();
    transportSource.addChangeListener(this);

    // Inizializzazione del AudioDeviceManager
    setAudioChannels(0, 2);

    setSize(800, 600);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
    transportSource.removeChangeListener(this);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    transportSource.getNextAudioBlock(bufferToFill);

    // Proteggi il buffer audio
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        protectYourEars(bufferToFill.buffer->getWritePointer(channel), bufferToFill.numSamples);
    }
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::releaseResources()
{
    transportSource.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);

    // Disegna la thumbnail
    auto thumbnailBounds = getLocalBounds().reduced(10);
    if (thumbnail.getTotalLength() > 0.0)
    {
        thumbnail.drawChannels(g, thumbnailBounds, 0.0, thumbnail.getTotalLength(), 1.0f);
    }
    else
    {
        g.setFont(20.0f);
        g.drawFittedText("No file loaded", thumbnailBounds, juce::Justification::centred, 1);
    }
}

void MainComponent::resized()
{
    importButton.setBounds(10, 10, 100, 30);
    exportButton.setBounds(120, 10, 100, 30);
    playButton.setBounds(230, 10, 100, 30);
    stopButton.setBounds(340, 10, 100, 30);
    volumeSlider.setBounds(10, 50, getWidth() - 20, 30);

    // Ridisegna la thumbnail
    repaint();
}

// Metodi extra

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &transportSource)
    {
        if (transportSource.isPlaying())
        {
            changeState(Playing);
        }
        else
        {
            if (state == Stopping)
                changeState(Stopped);
            else
                changeState(Stopped); // Assicurati che lo stato venga aggiornato correttamente
        }
    }
}

void MainComponent::buttonClicked(juce::Button* clickedButton)
{
    if (clickedButton == &importButton)
    {
        DBG("Import button clicked");
        auto chooser = std::make_shared<juce::FileChooser>("Seleziona un file audio da importare...", juce::File{}, "*.wav;*.mp3;*.aiff");
        chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this, chooser](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.exists())
                {
                    DBG("File selected: " + file.getFullPathName());
                    loadFile(file);
                }
                else
                {
                    DBG("No file selected");
                }
            });
    }
    else if (clickedButton == &exportButton)
    {
        DBG("Export button clicked");
        auto chooser = std::make_shared<juce::FileChooser>("Salva il file audio...", juce::File{}, "*.wav");
        chooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
            [this, chooser](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file != juce::File{})
                {
                    DBG("File selected for saving: " + file.getFullPathName());
                    exportFileWithVolume(file);
                }
                else
                {
                    DBG("No file selected for saving");
                }
            });
    }
    else if (clickedButton == &playButton)
    {
        DBG("Play button clicked");
        changeState(Starting);
    }
    else if (clickedButton == &stopButton)
    {
        DBG("Stop button clicked");
        changeState(Stopping);
    }
}

void MainComponent::changeState(TransportState newState)
{
    if (state != newState)
    {
        state = newState;

        switch (state)
        {
        case Stopped:
            playButton.setEnabled(true);
            stopButton.setEnabled(false);
            transportSource.setPosition(0.0);
            break;

        case Starting:
            transportSource.setPosition(0.0); // Reimposta la posizione all'inizio del file
            transportSource.start();
            break;

        case Playing:
            playButton.setEnabled(false);
            stopButton.setEnabled(true);
            break;

        case Stopping:
            transportSource.stop();
            break;
        }
    }
}

void MainComponent::loadFile(const juce::File& file)
{
    auto* reader = formatManager.createReaderFor(file);

    if (reader != nullptr)
    {
        std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader, true));
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());

        thumbnail.setSource(new juce::FileInputSource(file));

        // Forza l'aggiornamento della thumbnail
        repaint();
    }
    else
    {
        DBG("Errore nel caricamento del file: " + file.getFullPathName());
    }
}

void MainComponent::saveFile(const juce::File& file)
{
    DBG("Saving file: " + file.getFullPathName());
    // Logica per salvare il file audio
}

void MainComponent::updateThumbnail()
{
    if (readerSource.get() == nullptr)
        return;

    auto* reader = readerSource->getAudioFormatReader();
    if (reader == nullptr)
        return;

    // Resetta la thumbnail esistente
    thumbnail.reset(2, reader->sampleRate, reader->lengthInSamples);

    // Leggi i dati audio in blocchi più grandi
    const int blockSize = 8192; // Dimensione del blocco aumentata
    juce::AudioBuffer<float> buffer(2, blockSize);
    int64_t numSamples = reader->lengthInSamples;
    int64_t position = 0;

    while (position < numSamples)
    {
        int numSamplesToRead = (int)std::min<int64_t>(blockSize, numSamples - position);
        reader->read(&buffer, 0, numSamplesToRead, position, true, true);
        buffer.applyGain((float)volumeSlider.getValue());

        // Proteggi il buffer audio
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            protectYourEars(buffer.getWritePointer(channel), numSamplesToRead);
        }

        // Aggiungi il blocco alla thumbnail
        thumbnail.addBlock(position, buffer, 0, numSamplesToRead);
        position += numSamplesToRead;
    }

    // Forza l'aggiornamento della thumbnail
    repaint();
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        DBG("Volume slider value changed: " + juce::String(slider->getValue()));
        transportSource.setGain((float)slider->getValue());
        updateThumbnail(); // Aggiorna la thumbnail
    }
}

void MainComponent::applyAdvancedSmoothing(juce::AudioBuffer<float>& buffer, int numSamples)
{
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* samples = buffer.getWritePointer(channel);
        for (int i = 1; i < numSamples - 1; ++i)
        {
            // Smoothing avanzato usando una media mobile
            samples[i] = (samples[i - 1] + samples[i] + samples[i + 1]) / 3.0f;
        }
    }
}

void MainComponent::exportFileWithVolume(const juce::File& file)
{
    if (readerSource.get() == nullptr)
    {
        DBG("No file loaded to export");
        return;
    }

    auto* reader = readerSource->getAudioFormatReader();
    if (reader == nullptr)
    {
        DBG("Error reading the source file");
        return;
    }

    juce::WavAudioFormat wavFormat;
    std::unique_ptr<juce::AudioFormatWriter> writer(wavFormat.createWriterFor(new juce::FileOutputStream(file),
        reader->sampleRate,
        reader->numChannels,
        16,
        {},
        0));

    if (writer == nullptr)
    {
        DBG("Error creating the writer");
        return;
    }

    juce::AudioBuffer<float> buffer(2, 8192);
    int64_t numSamples = reader->lengthInSamples;
    int64_t position = 0;

    while (position < numSamples)
    {
        int numSamplesToRead = (int)std::min<int64_t>(buffer.getNumSamples(), numSamples - position);
        reader->read(&buffer, 0, numSamplesToRead, position, true, true);
        buffer.applyGain((float)volumeSlider.getValue());

        // Proteggi il buffer audio
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            protectYourEars(buffer.getWritePointer(channel), numSamplesToRead);
        }

        writer->writeFromAudioSampleBuffer(buffer, 0, numSamplesToRead);
        position += numSamplesToRead;
    }

    DBG("File exported successfully: " + file.getFullPathName());
}

























