/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.hpp"
#include "PluginEditor.hpp"

//==============================================================================
MckDelayAudioProcessor::MckDelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      )
#endif
{
    addParameter(time = new juce::AudioParameterInt("time", "Time", getMinTime(), getMaxTime(), 250));
    addParameter(feedback = new juce::AudioParameterInt("feedback", "Feedback", 0, 100, 25));
    addParameter(mix = new juce::AudioParameterInt("mix", "Mix", 0, 100, 50));
}

MckDelayAudioProcessor::~MckDelayAudioProcessor()
{
}

//==============================================================================
const juce::String MckDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MckDelayAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool MckDelayAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool MckDelayAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double MckDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MckDelayAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int MckDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MckDelayAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String MckDelayAudioProcessor::getProgramName(int index)
{
    return {};
}

void MckDelayAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

//==============================================================================
void MckDelayAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    numChannels = getTotalNumInputChannels();
    this->sampleRate = sampleRate;
    bufLen = static_cast<size_t>(std::ceil(static_cast<double>(getMaxTime()) / 1000.0 * sampleRate));
    delayBuffer.resize(numChannels);
    for (auto &buf : delayBuffer)
    {
        buf.resize(bufLen, 0.0);
    }

    m_delays.resize(numChannels);
    for (auto &dly : m_delays) {
        dly.prepareToPlay(sampleRate, samplesPerBlock);
    }

}

void MckDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MckDelayAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void MckDelayAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    /*
    if (m_editor != nullptr)
    {
        double time = m_editor->getTime();
        double feedback = m_editor->getFeedback() / 100.0;

        size_t len = buffer.getNumSamples();
    }*/
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    double wetMix = static_cast<double>(*mix) / 100.0;
    double wetFb = ((double)*feedback) / 100.0;
    size_t len = buffer.getNumSamples();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto *readPtr = buffer.getReadPointer(channel);
        auto *writePtr = buffer.getWritePointer(channel);

        m_delays[channel].setDelayInMs(static_cast<double>(*time));
        m_delays[channel].setMix(wetMix);
        m_delays[channel].setFeedback(wetFb);

        for (int s = 0; s < len; s++) {
            writePtr[s] = m_delays[channel].processSample(readPtr[s]);
        }
    }
    return;


    size_t writeLeft = bufLen - bufIdx;
    size_t writeSize = std::min(len, writeLeft);

    double directMix = 1.0 - wetMix;
    double directFb = 1.0 - wetFb;

    // Write To Delay Line
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto *readPtr = buffer.getReadPointer(channel);
        for (int s = 0; s < writeSize; s++)
        {
            delayBuffer[channel][bufIdx + s] = readPtr[s];
        }
        for (int s = writeSize; s < len; s++)
        {
            delayBuffer[channel][s - writeSize] = readPtr[s];
        }
    }

    int dlySamples = static_cast<size_t>(std::round(static_cast<double>(*time) / 1000.0 * sampleRate));
    int readIdx = (static_cast<int>(bufIdx) + bufLen - dlySamples) % bufLen;
    size_t readLeft = bufLen - readIdx;
    size_t readSize = std::min(len, readLeft);

    // Read From Delay Line
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto *readPtr = buffer.getReadPointer(channel);
        auto *writePtr = buffer.getWritePointer(channel);

        for (int s = 0; s < readSize; s++)
        {
            writePtr[s] = readPtr[s] * directMix + delayBuffer[channel][readIdx + s] * wetMix;
        }
        for (int s = readSize; s < len; s++)
        {
            writePtr[s] = readPtr[s] * directMix + delayBuffer[channel][s - readSize] * wetMix;
        }
    }

    bufIdx = (bufIdx + len) % bufLen;
}

//==============================================================================
bool MckDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *MckDelayAudioProcessor::createEditor()
{
    return new MckDelayAudioProcessorEditor(*this);
}

//==============================================================================
void MckDelayAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    std::unique_ptr<juce::XmlElement> xml(new juce::XmlElement("MckDelay"));
    xml->setAttribute("time", (double)*time);
    xml->setAttribute("feedback", (double)*feedback);
    xml->setAttribute("mix", (double)*mix);

    // juce::MemoryOutputStream(destData, true).writeInt(*time);
    // juce::MemoryOutputStream(destData, true).writeInt(*mix);
    // juce::MemoryOutputStream(destData, true).writeInt(*feedback);
}

void MckDelayAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    // *time = juce::MemoryInputStream(data, static_cast<size_t>(sizeInBytes), false).readInt();
    // *mix = juce::MemoryInputStream(data, static_cast<size_t>(sizeInBytes), false).readInt();
    // *feedback = juce::MemoryInputStream(data, static_cast<size_t>(sizeInBytes), false).readInt();

    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName("MckDelay"))
        {
            *time = xmlState->getIntAttribute("time", 250);
            *feedback = xmlState->getIntAttribute("feedback", 25);
            *mix = xmlState->getIntAttribute("mix", 50);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new MckDelayAudioProcessor();
}
