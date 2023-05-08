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
    
    addParameter(speed = new juce::AudioParameterFloat("speed", "Speed", getMinSpeed(), getMaxSpeed(), 0.1));
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

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    // size_t len = buffer.getNumSamples();
    // double wetMix = static_cast<double>(*mix) / 100.0;
    // double wetFb = ((double)*feedback) / 100.0;
    // double timeCoeff = (static_cast<double>(*time) - m_oldTime) / static_cast<double>(len);

    // for (size_t channel = 0; channel < std::min(totalNumInputChannels, totalNumOutputChannels); ++channel)
    // {
    //     auto *readPtr = buffer.getReadPointer(channel);
    //     auto *writePtr = buffer.getWritePointer(channel);

    //     m_delays[channel].setMix(wetMix);
    //     m_delays[channel].setFeedback(wetFb);
    //     m_delays[channel].setLowPass(*lpActive, static_cast<double>(*lpFreq));
    //     m_delays[channel].setHighPass(*hpActive, static_cast<double>(*hpFreq));

    //     for (size_t s = 0; s < len; s++) {
    //         m_delays[channel].setDelayInMs(m_oldTime + static_cast<double>(s) * timeCoeff);
    //         writePtr[s] = m_delays[channel].processSample(readPtr[s]);
    //     }
    // }

    // m_oldTime = static_cast<double>(*time);
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
            *speed = xmlState->getDoubleAttribute("speed", 0.1);
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
