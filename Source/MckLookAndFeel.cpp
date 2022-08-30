/**
 * @file MckLookAndFeel.cpp
 * @author HenriCSHC (henri.meissner@gmx.de)
 * @brief
 * @version 0.1
 * @date 2022-08-30
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "MckLookAndFeel.h"

MckLookAndFeel::MckLookAndFeel()
{
    setColour(juce::DocumentWindow::backgroundColourId, juce::Colours::floralwhite);
    setColour(juce::Label::textColourId, juce::Colours::darkslategrey);
    //setColour(juce::Slider::thumbColourId, juce::Colours::tomato);
    //setColour(juce::Slider::trackColourId, juce::Colours::darkorchid);
}

void MckLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                                      const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &)
{
    auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
    auto centreX = (float)x + (float)width * 0.5f;
    auto centreY = (float)y + (float)height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    juce::ColourGradient grad;
    grad = juce::ColourGradient::horizontal(juce::Colours::deepskyblue, x, juce::Colours::skyblue, x + width);

    // fill
    g.setGradientFill(grad);
    g.fillEllipse(rx, ry, rw, rw);

    // outline
    g.setColour(juce::Colours::darkslategrey);
    g.drawEllipse(rx, ry, rw, rw, 1.0f);

    juce::Path p;
    auto pointerLength = radius * 0.5f;
    auto pointerThickness = 4.0f;
    p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

    // pointer
    g.setColour(juce::Colours::darkslategrey);
    g.fillPath(p);

    // overlay
    juce::ColourGradient grad2(juce::Colours::darkgoldenrod, centreX, centreY, juce::Colours::darkslategrey, centreX - pointerLength, centreY - pointerLength, true);
    grad2.addColour(0.5, juce::Colours::darkslategrey);

    g.setColour(juce::Colours::darkslategrey);
    g.setGradientFill(grad2);
    g.fillEllipse(centreX - pointerLength, centreY - pointerLength, pointerLength * 2.0f, pointerLength * 2.0f);
    g.setColour(juce::Colours::darkslategrey);
    g.drawEllipse(centreX - pointerLength, centreY - pointerLength, pointerLength * 2.0f, pointerLength * 2.0f, 2.0f);
}