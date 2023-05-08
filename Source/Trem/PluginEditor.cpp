/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.hpp"
#include "PluginEditor.hpp"

#include <cstdio>

//==============================================================================
MckTremAudioProcessorEditor::MckTremAudioProcessorEditor(MckTremAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    p.setEditor(this);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    int w = 4 * dialSize + 5 * colGap;
    int h = headerHeight + dialSize + labelHeight + 2 * rowGap;
    setSize(w, h);
    setLookAndFeel(&mckLookAndFeel);
    LookAndFeel::setDefaultLookAndFeel(&mckLookAndFeel);

    controls.resize(4);
    controls[0].name = "Speed";
    controls[0].value = 2.0;
    controls[0].minVal = 0.1;
    controls[0].maxVal = 15.0;
    controls[0].stepVal = 0.1;
    controls[0].unit = " Hz";
    controls[1].name = "Shape";
    controls[1].value = 0.0;
    controls[1].minVal = 0.0;
    controls[1].maxVal = 100.0;
    controls[1].stepVal = 1.0;
    controls[1].unit = " %";
    controls[2].name = "Intensity";
    controls[2].value = 40.0;
    controls[2].minVal = 0.0;
    controls[2].maxVal = 100.0;
    controls[2].stepVal = 1.0;
    controls[2].unit = " %";
    controls[3].name = "Modulation";
    controls[3].value = 0.0;
    controls[3].minVal = 0.0;
    controls[3].maxVal = 100.0;
    controls[3].stepVal = 1.0;
    controls[3].unit = " %";

    labels.resize(controls.size());
    labels[0] = &speedLabel;
    labels[1] = &shapeLabel;
    labels[2] = &intensityLabel;
    labels[3] = &modulationLabel;

    int i = 0;
    for (auto &label : labels)
    {
        label->setText(controls[i].name, juce::NotificationType::dontSendNotification);
        label->setFont(juce::Font(labelHeight, juce::Font::plain));
        label->setColour(juce::Label::textColourId, juce::Colour::fromRGB(230, 230, 230));
        label->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(*label);
        i++;
    }

    sliders.resize(controls.size());
    sliders[0] = &speedSlider;
    sliders[1] = &shapeSlider;
    sliders[2] = &intensitySlider;
    sliders[3] = &modulationSlider;

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

    speedSlider.setValue(static_cast<double>(audioProcessor.getSpeed()));
    shapeSlider.setValue(static_cast<double>(audioProcessor.getShape()));
    intensitySlider.setValue(static_cast<double>(audioProcessor.getIntensity()));
    modulationSlider.setValue(static_cast<double>(audioProcessor.getModulation()));

    resized();
}

MckTremAudioProcessorEditor::~MckTremAudioProcessorEditor()
{
    setLookAndFeel(nullptr);

    for (auto &slider : sliders)
    {
        slider->removeListener(this);
    }
    sliders.clear();
    labels.clear();
}

//==============================================================================
void MckTremAudioProcessorEditor::paint(juce::Graphics &g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    auto bgColor = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    g.fillAll(bgColor);

    auto bounds = getLocalBounds();
    auto w = bounds.getWidth();
    bounds.setHeight(headerHeight - 2 * headerGap);
    g.setColour(juce::Colour::fromRGB(0, 155, 179));
    g.fillRect(bounds);
    bounds.setTop(headerHeight - headerGap);
    bounds.setHeight(headerGap);
    g.fillRect(bounds);

    // XML Stuff
    auto svgBounds = getLocalBounds().toFloat();
    svgBounds.setTop(2.0f * headerGap);
    svgBounds.setHeight(headerHeight - (2.0f + 4.0f) * headerGap);
    svgBounds.setLeft(8.0f);
    svgBounds.setWidth(w / 2 - 16.0f);
    // svgBounds.setWidth(svgBounds.getHeight() * 4.0f);
    auto svgXml = XmlDocument::parse(BinaryData::mckaudio_logo_svg);
    auto svgDrawable = Drawable::createFromSVG(*svgXml);
    svgDrawable->setTransformToFit(svgBounds, RectanglePlacement::xLeft || RectanglePlacement::yTop);
    svgDrawable->draw(g, 1.0f);

    auto fontBounds = getLocalBounds().toFloat();
    fontBounds.setTop(2.0f * headerGap);
    fontBounds.setHeight(headerHeight - (2.0f + 3.0f) * headerGap);
    fontBounds.setLeft(w / 2.0f);
    fontBounds.setRight(w - 8.0f);

    g.setFont(juce::Font(fontSize, juce::Font::plain));
    g.setColour(bgColor);
    g.drawFittedText("Tremolo", fontBounds.toNearestInt(), juce::Justification::bottomRight, 1);
}

void MckTremAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto len = controls.size();
    auto bounds = getLocalBounds();
    bounds.setTop(headerHeight);
    bounds.setHeight(dialSize + 1 * rowGap);
    bounds.setWidth(dialSize * len + colGap * (len + 1));

    juce::FlexBox sliderBox;
    sliderBox.flexDirection = juce::FlexBox::Direction::row;
    for (auto &slider : sliders)
    {
        sliderBox.items.add(juce::FlexItem(*slider).withFlex(0, 1, dialSize).withMargin(juce::FlexItem::Margin(rowGap, colGap, 0, colGap)));
    }
    sliderBox.performLayout(bounds.toFloat());

    bounds.setTop(headerHeight + dialSize + 1 * rowGap);
    bounds.setHeight(labelHeight + rowGap);

    juce::FlexBox labelBox;
    labelBox.flexDirection = juce::FlexBox::Direction::row;
    for (auto &label : labels)
    {
        labelBox.items.add(juce::FlexItem(*label).withFlex(0, 1, dialSize).withMargin(juce::FlexItem::Margin(0, colGap, rowGap, colGap)));
    }
    labelBox.performLayout(bounds.toFloat());
}

void MckTremAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &speedSlider)
    {
        audioProcessor.setSpeed(slider->getValue());
    }
    else if (slider == &shapeSlider)
    {
        audioProcessor.setShape(static_cast<int>(std::round(slider->getValue())));
    }
    else if (slider == &intensitySlider)
    {
        audioProcessor.setIntensity(static_cast<int>(std::round(slider->getValue())));
    }
    else if (slider == &modulationSlider)
    {
        audioProcessor.setModulation(static_cast<int>(std::round(slider->getValue())));
    }
}

void MckTremAudioProcessorEditor::parameterValueChanged(int parameterIndex, float newValue)
{
    switch (parameterIndex)
    {
    case 0:
        speedSlider.setValue(static_cast<double>(audioProcessor.getSpeed()), NotificationType::dontSendNotification);
        break;
    case 1:
        shapeSlider.setValue(static_cast<double>(audioProcessor.getShape()), NotificationType::dontSendNotification);
        break;
    case 2:
        intensitySlider.setValue(static_cast<double>(audioProcessor.getIntensity()), NotificationType::dontSendNotification);
        break;
    case 3:
        modulationSlider.setValue(static_cast<double>(audioProcessor.getModulation()), NotificationType::dontSendNotification);
        break;
    default:
        break;
    }
}
void MckTremAudioProcessorEditor::parameterGestureChanged(int parameterIndex, bool gestureIsStarting)
{
}