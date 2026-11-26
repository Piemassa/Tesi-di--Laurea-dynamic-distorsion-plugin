#pragma once
#include <JuceHeader.h>

class LookAndFeelKnob : public juce::LookAndFeel_V4
{
public:
    LookAndFeelKnob()
    {
        setColour (juce::Slider::rotarySliderFillColourId,    juce::Colour (Colours::yellowgreen)); // arco valore
        setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff2b2f36)); // binario
        setColour (juce::Slider::thumbColourId,               juce::Colours::white);      // indicatore
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float startAngle, float endAngle,
                           juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (6.0f);
        const float radius    = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
        const auto  centre    = bounds.getCentre();
        const float angle     = startAngle + sliderPos * (endAngle - startAngle);
        const float thickness = radius * 0.14f;
        const float arcRadius = radius - thickness * 0.5f;

        // --- binario (arco di sfondo)
        juce::Path track;
        track.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                             startAngle, endAngle, true);
        g.setColour (slider.findColour (juce::Slider::rotarySliderOutlineColourId));
        g.strokePath (track, juce::PathStrokeType (thickness,
                                                   juce::PathStrokeType::curved,
                                                   juce::PathStrokeType::rounded));

        // --- arco del valore (dal centro se il range è bipolare)
        float fromAngle = startAngle;
        if (slider.getMinimum() < 0.0 && slider.getMaximum() > 0.0)
        {
            const auto zeroPos = (float) slider.valueToProportionOfLength (0.0);
            fromAngle = startAngle + zeroPos * (endAngle - startAngle);
        }

        juce::Path valueArc;
        valueArc.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                                juce::jmin (fromAngle, angle),
                                juce::jmax (fromAngle, angle), true);
        g.setColour (slider.findColour (juce::Slider::rotarySliderFillColourId));
        g.strokePath (valueArc, juce::PathStrokeType (thickness,
                                                      juce::PathStrokeType::curved,
                                                      juce::PathStrokeType::rounded));

        // --- corpo della manopola
        const float bodyRadius = radius - thickness * 2.2f;
        juce::ColourGradient body (juce::Colour (0xff4a4f58), centre.x, centre.y - bodyRadius,
                                   juce::Colour (0xff22252b), centre.x, centre.y + bodyRadius, false);
        g.setGradientFill (body);
        g.fillEllipse (centre.x - bodyRadius, centre.y - bodyRadius,
                       bodyRadius * 2.0f, bodyRadius * 2.0f);

        g.setColour (juce::Colours::black.withAlpha (0.5f));
        g.drawEllipse (centre.x - bodyRadius, centre.y - bodyRadius,
                       bodyRadius * 2.0f, bodyRadius * 2.0f, 1.0f);

        // --- indicatore
        juce::Path pointer;
        pointer.addRoundedRectangle (-1.5f, -bodyRadius + 3.0f, 3.0f, bodyRadius * 0.45f, 1.5f);
        pointer.applyTransform (juce::AffineTransform::rotation (angle).translated (centre));
        g.setColour (slider.findColour (juce::Slider::thumbColourId));
        g.fillPath (pointer);
    }
};
