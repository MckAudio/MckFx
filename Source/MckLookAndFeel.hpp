#pragma once

#include <JuceHeader.h>

class MckLookAndFeel : public juce::LookAndFeel_V4
{
public:
    MckLookAndFeel();

    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &) override;

    void fillResizableWindowBackground(juce::Graphics &g,
                                       int width,
                                       int height,
                                       const juce::BorderSize<int> &borderSize,
                                       juce::ResizableWindow &window) override;
};