#include "BwLookAndFeel.hpp"

BwLookAndFeel::BwLookAndFeel()
{
    setColour(juce::DocumentWindow::backgroundColourId, juce::Colour::fromRGB(65,65,65));
    setColour(juce::Label::textColourId, juce::Colour::fromRGB(242,242,242));
}

void BwLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                                      const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &)
{
    auto size = (float)juce::jmin(width / 2, height / 2);
    auto knobRadius = 0.75f * size;
    auto radius = size - 4.0f;
    auto centreX = (float)x + (float)width * 0.5f;
    auto centreY = (float)y + (float)height * 0.5f;
    auto rx = centreX - knobRadius;
    auto ry = centreY - knobRadius;
    auto rw = knobRadius * 2.0f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);


    juce::ColourGradient grad;
    grad = juce::ColourGradient::vertical(juce::Colour::fromRGB(160, 160, 160), y, juce::Colour::fromRGB(120, 120, 120), y + height);
    grad.addColour(0.5, juce::Colour::fromRGB(120, 120, 120));
    grad.addColour(0.75, juce::Colour::fromRGB(105, 105, 105));

    // fill
    g.setGradientFill(grad);
    g.fillEllipse(rx, ry, rw, rw);

    // pointer
    juce::Path p1;
    auto pointerLength = knobRadius * 0.75f;
    auto pointerThickness = size / 10.0f;
    p1.addRectangle(-pointerThickness * 0.5f, -knobRadius + pointerThickness * 0.5f, pointerThickness, pointerLength - pointerThickness);
    p1.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

    g.setColour(juce::Colour::fromRGB(242,242,242));
    g.fillPath(p1);


    // outline
    auto outlineThickness = size / 20.0f;
    g.setColour(juce::Colour::fromRGB(40, 40, 40));
    g.drawEllipse(rx + outlineThickness * 0.5f, ry + outlineThickness * 0.5f, rw - outlineThickness, rw - outlineThickness, outlineThickness);

    // Path
    auto pathThickness = pointerThickness;
    auto pathRadius = size - pointerThickness;

    juce::Path p2;
    p2.addCentredArc(centreX, centreY, size, size, 0, rotaryStartAngle, rotaryEndAngle, true);
    p2.addCentredArc(centreX, centreY, pathRadius, pathRadius, 0, rotaryEndAngle, rotaryStartAngle);
    p2.closeSubPath();

    g.setColour(juce::Colour::fromRGB(100, 100, 100));
    g.fillPath(p2);


    juce::Path p3;
    auto curAngle = rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * sliderPos;
    p3.addCentredArc(centreX, centreY, size, size, 0, rotaryStartAngle, curAngle, true);
    p3.addCentredArc(centreX, centreY, pathRadius, pathRadius, 0, curAngle, rotaryStartAngle);
    p3.closeSubPath();

    g.setColour(juce::Colour::fromRGB(255, 125, 15));
    g.fillPath(p3);
}