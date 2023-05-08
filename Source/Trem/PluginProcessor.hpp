/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include "../Dsp/LfoModule.hpp"

class MckTremAudioProcessorEditor;

//==============================================================================
/**
 */
class MckTremAudioProcessor : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
    ,
                               public juce::AudioProcessorARAExtension
#endif
{
public:
  //==============================================================================
  MckTremAudioProcessor();
  ~MckTremAudioProcessor() override;

  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  //==============================================================================
  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override;

  //==============================================================================
  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String &newName) override;

  //==============================================================================
  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  void setEditor(MckTremAudioProcessorEditor *editor);

  double getMinSpeed() { return 0.1; };
  double getMaxSpeed() { return 15.0; };

  void setSpeed(double s) { *speed = s; };
  double getSpeed() { return *speed; };

  void setShape(int s) { *shape = s; };
  int getShape() { return *shape; };

  void setIntensity(int i) { *intensity = i; };
  int getIntensity() { return *intensity; };

  void setModulation(int m) { *modulation = m; };
  int getModulation() { return *modulation; };

private:
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MckTremAudioProcessor)

  MckTremAudioProcessorEditor *m_editor{nullptr};

  juce::AudioParameterFloat *speed;
  juce::AudioParameterInt *shape;
  juce::AudioParameterInt *intensity;
  juce::AudioParameterInt *modulation;

  size_t numChannels { 0 };

  MckDsp::LfoModule lfo;
  MckDsp::LfoModule mod;
};
