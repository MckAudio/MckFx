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
    juce::AudioParameterFloatAttributes freqAttr;
    freqAttr.withLabel("Hz");
    juce::NormalisableRange<float> freqRange(20.0f, 20000.0f, 0.1f, 0.5f);
    freqRange.setSkewForCentre(1000.0f);
    
    addParameter(time = new juce::AudioParameterInt("time", "Time", getMinTime(), getMaxTime(), 250, juce::AudioParameterIntAttributes().withLabel("ms")));
    addParameter(feedback = new juce::AudioParameterInt("feedback", "Feedback", 0, 100, 25, juce::AudioParameterIntAttributes().withLabel("%")));
    addParameter(mix = new juce::AudioParameterInt("mix", "Mix", 0, 100, 50, juce::AudioParameterIntAttributes().withLabel("%")));
    addParameter(lpActive = new juce::AudioParameterBool("lpactive", "Low Pass Active", false));
    addParameter(lpFreq = new juce::AudioParameterFloat("lpfreq", "Low Pass Frequency", freqRange, 1000, freqAttr));
    addParameter(hpActive = new juce::AudioParameterBool("hpactive", "High Pass Active", false));
    addParameter(hpFreq = new juce::AudioParameterFloat("hpfreq", "High Pass Frequency", freqRange, 1000, freqAttr));
    
    /*
    juce::AudioParameterFloatAttributes freqAttr;
    freqAttr.withLabel("Hz");
    freqAttr.withCategory(juce::AudioProcessorParameter::Category::genericParameter);
    freqAttr.withStringFromValueFunction([](float value, int maximumStringLength) -> juce::String {
        return std::to_string(std::round(value * 100.0f) / 100.0f) + " Hertz";
    });
    freqAttr.withValueFromStringFunction([](const juce::String &str) -> float {
        //float val = str.getFloatValue();
        return 42.0f;
    });

    juce::NormalisableRange<float> freqRange(20.0, 20000.0, [](float rangeStart, float rangeEnd, float valueToRemap) -> float { return 20.0f * std::pow(10.0f, 3.0f * valueToRemap); }, [](float rangeStart, float rangeEnd, float valueToRemap) -> float { return std::log10(valueToRemap/20.0f) / 3.0f; });
    */

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

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    size_t len = buffer.getNumSamples();
    double wetMix = static_cast<double>(*mix) / 100.0;
    double wetFb = ((double)*feedback) / 100.0;
    double timeCoeff = (static_cast<double>(*time) - m_oldTime) / static_cast<double>(len);

    for (size_t channel = 0; channel < std::min(totalNumInputChannels, totalNumOutputChannels); ++channel)
    {
        auto *readPtr = buffer.getReadPointer(channel);
        auto *writePtr = buffer.getWritePointer(channel);

        m_delays[channel].setMix(wetMix);
        m_delays[channel].setFeedback(wetFb);
        m_delays[channel].setLowPass(*lpActive, static_cast<double>(*lpFreq));
        m_delays[channel].setHighPass(*hpActive, static_cast<double>(*hpFreq));

        for (size_t s = 0; s < len; s++) {
            m_delays[channel].setDelayInMs(m_oldTime + static_cast<double>(s) * timeCoeff);
            writePtr[s] = m_delays[channel].processSample(readPtr[s]);
        }
    }

    m_oldTime = static_cast<double>(*time);
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
    xml->setAttribute("lpactive", (double)*lpActive);
    xml->setAttribute("lpfreq", (double)*lpFreq);
    xml->setAttribute("hpactive", (double)*hpActive);
    xml->setAttribute("hpfreq", (double)*hpFreq);
    copyXmlToBinary(*xml, destData);

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
            *lpActive = xmlState->getBoolAttribute("lpactive", false);
            *lpFreq = static_cast<float>(xmlState->getDoubleAttribute("lpfreq", 1000));
            *hpActive = xmlState->getBoolAttribute("hpactive", false);
            *hpFreq = static_cast<float>(xmlState->getDoubleAttribute("hpfreq", 1000));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new MckDelayAudioProcessor();
}
