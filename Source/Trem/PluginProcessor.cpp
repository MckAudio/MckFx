/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.hpp"
#include "PluginEditor.hpp"

//==============================================================================
MckTremAudioProcessor::MckTremAudioProcessor()
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
    juce::AudioParameterFloatAttributes freqAttr;
    freqAttr.withLabel("Hz");
    juce::NormalisableRange<float> freqRange(20.0f, 20000.0f, 0.1f, 0.5f);
    freqRange.setSkewForCentre(1000.0f);

    addParameter(speed = new juce::AudioParameterFloat("speed", "Speed", getMinSpeed(), getMaxSpeed(), 2.0));
    addParameter(shape = new juce::AudioParameterInt("shape", "Shape", 0, 100, 0, juce::AudioParameterIntAttributes().withLabel("%")));
    addParameter(intensity = new juce::AudioParameterInt("intensity", "Intensity", 0, 100, 40, juce::AudioParameterIntAttributes().withLabel("%")));
}

MckTremAudioProcessor::~MckTremAudioProcessor()
{
}

//==============================================================================
const juce::String MckTremAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MckTremAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool MckTremAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool MckTremAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double MckTremAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MckTremAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int MckTremAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MckTremAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String MckTremAudioProcessor::getProgramName(int index)
{
    return {};
}

void MckTremAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

//==============================================================================
void MckTremAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    numChannels = getTotalNumInputChannels();
    lfo.prepareToPlay(sampleRate, samplesPerBlock);
}

void MckTremAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MckTremAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
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

void MckTremAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    double _speed = 0.0;
    double _shape = 0.0;
    double _intensity = 1.0;
    double _lfo = 0.0;
    for (size_t s = 0; s < buffer.getNumSamples(); s++)
    {
        _speed = *speed;
        _shape = static_cast<double>(*shape) / 100.0;
        _intensity = static_cast<double>(*intensity) / 100.0;

        lfo.setSpeed(_speed);
        lfo.setShape(_shape);
        _lfo = lfo.processSample();

        for (size_t c = 0; c < std::min(totalNumInputChannels, totalNumOutputChannels); c++)
        {
            auto *readPtr = buffer.getReadPointer(c);
            auto *writePtr = buffer.getWritePointer(c);

            writePtr[s] = readPtr[s] * ((1.0 - _intensity) + _intensity * _lfo);
        }
    }
}

//==============================================================================
bool MckTremAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *MckTremAudioProcessor::createEditor()
{
    return new MckTremAudioProcessorEditor(*this);
}

//==============================================================================
void MckTremAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    std::unique_ptr<juce::XmlElement> xml(new juce::XmlElement("MckTrem"));
    xml->setAttribute("speed", (double)*speed);
    xml->setAttribute("shape", (double)*shape);
    xml->setAttribute("intensity", (double)*intensity);
    copyXmlToBinary(*xml, destData);
}

void MckTremAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    // *time = juce::MemoryInputStream(data, static_cast<size_t>(sizeInBytes), false).readInt();
    // *mix = juce::MemoryInputStream(data, static_cast<size_t>(sizeInBytes), false).readInt();
    // *feedback = juce::MemoryInputStream(data, static_cast<size_t>(sizeInBytes), false).readInt();

    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName("MckTrem"))
        {
            *speed = xmlState->getDoubleAttribute("speed", 2.0);
            *shape = xmlState->getIntAttribute("shape", 0);
            *intensity = xmlState->getIntAttribute("intensity", 40);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new MckTremAudioProcessor();
}
