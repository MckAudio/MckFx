/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.hpp"
#include "PluginEditor.hpp"

#include <cstdio>

//==============================================================================
MckDelayAudioProcessorEditor::MckDelayAudioProcessorEditor(MckDelayAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    p.setEditor(this);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(240, 120);
    setResizeLimits(200, 100, 1200, 900);
    setResizable(true, true);

    setLookAndFeel(&bwLookAndFeel);

    controls.resize(3);
    controls[0].name = "Time";
    controls[0].value = 300.0;
    controls[0].minVal = 100.0;
    controls[0].maxVal = 1000.0;
    controls[0].stepVal = 1.0;
    controls[0].unit = " ms";
    controls[1].name = "Feedback";
    controls[1].value = 25.0;
    controls[1].minVal = 0.0;
    controls[1].maxVal = 100.0;
    controls[1].stepVal = 1.0;
    controls[1].unit = " %";
    controls[2].name = "Mix";
    controls[2].value = 50.0;
    controls[2].minVal = 0.0;
    controls[2].maxVal = 100.0;
    controls[2].stepVal = 1.0;
    controls[2].unit = " %";

    labels.resize(controls.size());
    labels[0] = &timeLabel;
    labels[1] = &fbLabel;
    labels[2] = &mixLabel;

    int i = 0;
    for (auto &label : labels)
    {
        label->setText(controls[i].name, juce::NotificationType::dontSendNotification);
        label->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(*label);
        i++;
    }

    sliders.resize(3);
    sliders[0] = &timeSlider;
    sliders[1] = &fbSlider;
    sliders[2] = &mixSlider;

    i = 0;
    for (auto &slider : sliders)
    {
        slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 40);
        slider->setPopupDisplayEnabled(true, true, this, 1000);
        slider->setTextValueSuffix(controls[i].unit);
        slider->setRange(controls[i].minVal, controls[i].maxVal, controls[i].stepVal);
        slider->setValue(controls[i].value);
        slider->setDoubleClickReturnValue(true, controls[i].value);
        slider->addListener(this);
        addAndMakeVisible(*slider);
        i++;
    }

    timeSlider.setValue(static_cast<double>(audioProcessor.getTime()));
    fbSlider.setValue(static_cast<double>(audioProcessor.getFeedback()));
    mixSlider.setValue(static_cast<double>(audioProcessor.getMix()));

    resized();
}

MckDelayAudioProcessorEditor::~MckDelayAudioProcessorEditor()
{
    setLookAndFeel(nullptr);

    for(auto &slider : sliders)
    {
        slider->removeListener(this);
    }
    sliders.clear();
    labels.clear();
}

//==============================================================================
void MckDelayAudioProcessorEditor::paint(juce::Graphics &g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // g.setColour (juce::Colours::white);
    // g.setFont (15.0f);
    // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void MckDelayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    /*
    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::row;

    for (auto &slider : sliders)
    {
      fb.items.add(juce::FlexItem(*(slider.get())).withFlex(0, 1, (float)getWidth() / (float) sliders.size()));
    }

    fb.performLayout(getLocalBounds().toFloat());
    */

    juce::Grid grid;
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    grid.templateRows = {Track(Fr(1)), Track(Fr(9))};
    grid.templateColumns = {Track(Fr(1)), Track(Fr(1)), Track(Fr(1))};

    for (auto &label : labels)
    {
        grid.items.add(juce::GridItem(*label));
    }

    for (auto &slider : sliders)
    {
        grid.items.add(juce::GridItem(*slider));
    }

    grid.performLayout(getLocalBounds());
}

void MckDelayAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &timeSlider) {
        audioProcessor.setTime(static_cast<int>(std::round(slider->getValue())));
    } else if (slider == &mixSlider) {
        audioProcessor.setMix(static_cast<int>(std::round(slider->getValue())));
    } else if (slider == &fbSlider) {
        audioProcessor.setFeedback(static_cast<int>(std::round(slider->getValue())));
    }
}