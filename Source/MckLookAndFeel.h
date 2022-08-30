/**
 * @file MckLookAndFeel.h
 * @author HenriCSHC (henri.meissner@gmx.de)
 * @brief
 * @version 0.1
 * @date 2022-08-30
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <JuceHeader.h>

class MckLookAndFeel : public juce::LookAndFeel_V4
{
public:
    MckLookAndFeel();

    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &) override;
};