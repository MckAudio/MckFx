/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.hpp"
#include "../Types/Control.hpp"
#include <MckLookAndFeel.hpp>

#include <vector>
#include <memory>
#include <string>

//==============================================================================
/**
 */
class MckTremAudioProcessorEditor : public juce::AudioProcessorEditor,
                                     public juce::Slider::Listener,
                                     public juce::AudioProcessorParameter::Listener
{
public:
  MckTremAudioProcessorEditor(MckTremAudioProcessor &);
  ~MckTremAudioProcessorEditor() override;

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
  
  void parameterValueChanged (int parameterIndex, float newValue) override;
  void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override;

private:
  void sliderValueChanged(juce::Slider *slider) override;

  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  MckTremAudioProcessor &audioProcessor;

  MckLookAndFeel mckLookAndFeel;

  std::vector<Data::Control> controls;

  juce::Label speedLabel;
  juce::Label shapeLabel;
  juce::Label intensityLabel;
  juce::Label modulationLabel;
  juce::Slider speedSlider;
  juce::Slider shapeSlider;
  juce::Slider intensitySlider;
  juce::Slider modulationSlider;

  std::vector<juce::Slider *> sliders;
  std::vector<juce::Label *> labels;

  const int headerHeight = 40;
  const int headerGap = 2;
  const int dialSize = 80;
  const int dialGap = 8;
  const int labelHeight = 16;
  const int fontSize = 20;
  const int rowGap = 4;
  const int colGap = 8;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MckTremAudioProcessorEditor)
};
