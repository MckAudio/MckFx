/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.hpp"
#include "Control.hpp"
#include "BwLookAndFeel.hpp"
#include "MckLookAndFeel.hpp"

#include <vector>
#include <memory>
#include <string>

//==============================================================================
/**
 */
class MckDelayAudioProcessorEditor : public juce::AudioProcessorEditor,
                                     public juce::Slider::Listener
{
public:
  MckDelayAudioProcessorEditor(MckDelayAudioProcessor &);
  ~MckDelayAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

  double getTime()
  {
    return sliders[0]->getValue();
  }
  double getFeedback()
  {
    return sliders[1]->getValue();
  }
  double getMix()
  {
    return sliders[2]->getValue();
  }

private:
  void sliderValueChanged(juce::Slider *slider) override;

  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  MckDelayAudioProcessor &audioProcessor;

  BwLookAndFeel bwLookAndFeel;
  MckLookAndFeel mckLookAndFeel;

  std::vector<Data::Control> controls;

  juce::Label timeLabel;
  juce::Label mixLabel;
  juce::Label fbLabel;
  juce::Slider timeSlider;
  juce::Slider mixSlider;
  juce::Slider fbSlider;

  std::vector<juce::Slider *> sliders;
  std::vector<juce::Label *> labels;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MckDelayAudioProcessorEditor)
};
