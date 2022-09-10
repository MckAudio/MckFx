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
    int w = 3 * dialSize + 4 * colGap;
    int h = headerHeight + dialSize + labelHeight + 2 * rowGap;
    setSize(w, h);
    // setResizeLimits(200, 100, 1200, 900);
    //setResizable(true, true);

    //setLookAndFeel(&bwLookAndFeel);
    setLookAndFeel(&mckLookAndFeel);
    LookAndFeel::setDefaultLookAndFeel(&mckLookAndFeel);

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
        label->setFont(juce::Font(labelHeight, juce::Font::plain));
        label->setColour(juce::Label::textColourId, juce::Colour::fromRGB(230, 230, 230));
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

    for (auto &slider : sliders)
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
    svgBounds.setWidth(w - 16.0f);
    //svgBounds.setWidth(svgBounds.getHeight() * 4.0f);
    auto svgXml = XmlDocument::parse(BinaryData::mckaudio_logo_svg);
    auto svgDrawable = Drawable::createFromSVG(*svgXml);
    svgDrawable->setTransformToFit(svgBounds, RectanglePlacement::xLeft || RectanglePlacement::yTop);
    svgDrawable->draw(g, 1.0f);
    

    // g.setColour (juce::Colours::white);
    // g.setFont (15.0f);
    // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void MckDelayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds();
    bounds.setTop(headerHeight);
    bounds.setHeight(dialSize + 1 * rowGap);
    bounds.setWidth(dialSize * 3 + colGap * 4);

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

    /*
        juce::Grid grid;
        using Track = juce::Grid::TrackInfo;
        using Fr = juce::Grid::Fr;

        grid.templateRows = {Track(Fr(1))};
        grid.templateColumns = {Track(1), Track(Fr(1)), Track(Fr(1))};


        for (auto &slider : sliders)
        {
            grid.items.add(juce::GridItem(*slider).withMargin(juce::GridItem::Margin(8.0f)));
        }

        grid.performLayout(bounds);*/
}

void MckDelayAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &timeSlider)
    {
        audioProcessor.setTime(static_cast<int>(std::round(slider->getValue())));
    }
    else if (slider == &mixSlider)
    {
        audioProcessor.setMix(static_cast<int>(std::round(slider->getValue())));
    }
    else if (slider == &fbSlider)
    {
        audioProcessor.setFeedback(static_cast<int>(std::round(slider->getValue())));
    }
}