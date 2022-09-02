/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include "DelayModule.hpp"

class MckDelayAudioProcessorEditor;

//==============================================================================
/**
 */
class MckDelayAudioProcessor : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
    ,
                               public juce::AudioProcessorARAExtension
#endif
{
public:
  //==============================================================================
  MckDelayAudioProcessor();
  ~MckDelayAudioProcessor() override;

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

  void setEditor(MckDelayAudioProcessorEditor *editor){m_editor = editor; };

  void setTime(int t) { *time = t; };
  int getTime() { return *time; };
  int getMinTime() { return 100; };
  int getMaxTime() { return 1000; };

  void setMix(int m) { *mix = m; };
  int getMix() { return *mix; };

  void setFeedback(int f) { *feedback = f; };
  int getFeedback() { return *feedback; };

private:
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MckDelayAudioProcessor)

  MckDelayAudioProcessorEditor *m_editor{nullptr};

  juce::AudioParameterInt *time;
  juce::AudioParameterInt *feedback;
  juce::AudioParameterInt *mix;
  juce::AudioParameterBool *lpActive;
  juce::AudioParameterInt *lpFreq;
  juce::AudioParameterBool *hpActive;
  juce::AudioParameterInt *hpFreq;

  double m_oldTime { 0.0 };

  std::vector<MckDsp::DelayModule> m_delays;
  size_t numChannels { 0 };
};
