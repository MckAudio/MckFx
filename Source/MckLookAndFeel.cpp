#include "MckLookAndFeel.hpp"

MckLookAndFeel::MckLookAndFeel()
{
    setColour(juce::DocumentWindow::backgroundColourId, juce::Colour::fromRGB(230, 230, 230));
    setColour(juce::Label::textColourId, juce::Colour::fromRGB(0, 66, 76));
}

void MckLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
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

    // auto hlColour = juce::Colour::fromRGB(0, 66, 76);
    auto bgColour = juce::Colour::fromRGB(230, 230, 230);
    auto shade1 = juce::Colour::fromRGB(224, 224, 224);
    auto shade2 = juce::Colour::fromRGB(190, 190, 190);
    auto shade3 = juce::Colour::fromRGB(160, 160, 160);
    auto ringColour = juce::Colour::fromRGB(200, 200, 200);
    auto ringHlColour = juce::Colour::fromRGB(0, 155, 179);

    auto hlColour = juce::Colour::fromRGB(0, 133, 153);
    auto hlShade1 = juce::Colour::fromRGB(0, 66, 76);
    auto hlShade2 = juce::Colour::fromRGB(0, 43, 51);

    // Knob Fill
    juce::ColourGradient grad(shade3, juce::Point<float>(centreX, centreY), bgColour, juce::Point<float>(x, y), true);
    g.setGradientFill(grad);
    g.fillEllipse(rx, ry, rw, rw);

    auto outlineThickness = std::ceil(size / 50.0f);

    // pointer
    juce::Path p1;
    auto pointerLength = knobRadius * 0.75f;
    auto pointerThickness = size / 10.0f;
    p1.addRectangle(-pointerThickness * 0.5f, -knobRadius, pointerThickness, pointerLength + pointerThickness);
    p1.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

    g.setColour(hlShade2);
    g.fillPath(p1);

    juce::Path p4;
    auto p4Thickness = size / 20.0f;
    p4.addRectangle(-p4Thickness * 0.5f, -knobRadius, p4Thickness, pointerLength + pointerThickness - p4Thickness / 2.0f);
    p4.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

    g.setColour(hlColour);
    g.fillPath(p4);

    // outline
    g.setColour(hlShade2);
    g.drawEllipse(rx + outlineThickness * 0.5f, ry + outlineThickness * 0.5f, rw - outlineThickness, rw - outlineThickness, outlineThickness);

    // Path
    auto pathThickness = pointerThickness;
    auto pathRadius = size - pointerThickness;

    juce::Path p2;
    p2.addCentredArc(centreX, centreY, size, size, 0, rotaryStartAngle, rotaryEndAngle, true);
    p2.addCentredArc(centreX, centreY, pathRadius, pathRadius, 0, rotaryEndAngle, rotaryStartAngle);
    p2.closeSubPath();

    g.setColour(ringColour);
    g.fillPath(p2);

    juce::Path p3;
    auto curAngle = rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * sliderPos;
    p3.addCentredArc(centreX, centreY, size, size, 0, rotaryStartAngle, curAngle, true);
    p3.addCentredArc(centreX, centreY, pathRadius, pathRadius, 0, curAngle, rotaryStartAngle);
    p3.closeSubPath();

    g.setColour(hlColour);
    g.fillPath(p3);
}

void MckLookAndFeel::fillResizableWindowBackground(juce::Graphics &g,
                                                   int width,
                                                   int height,
                                                   const juce::BorderSize<int> &borderSize,
                                                   juce::ResizableWindow &window)
{
    g.setColour(juce::Colour::fromRGB(255,127,0));
    g.fillRect(0, 0, width, height);
}