#pragma once

#include <JuceHeader.h>

// Knob rotativo + etichetta + attachment in un solo componente.
class LabeledKnob : public juce::Component
{
public:
    LabeledKnob (juce::AudioProcessorValueTreeState& apvts,
                 const juce::String& paramID,
                 const juce::String& labelText,
                 bool showTextBox = true)
        : attachment (apvts, paramID, slider),textBoxHeight (showTextBox ? 18 : 0)   // slider è già costruito (dichiarato prima)
    {
        slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle (showTextBox ? juce::Slider::TextBoxBelow
                                            : juce::Slider::NoTextBox,
                                false, 60, 18);

        label.setText (labelText, juce::dontSendNotification);
        label.setJustificationType (juce::Justification::centred);

        addAndMakeVisible (slider);
        addAndMakeVisible (label);
    }

    void resized() override
    {
        auto r = getLocalBounds();
        const int dial = jmax (0, jmin (r.getWidth(), r.getHeight() - labelHeight - textBoxHeight));
        auto group = r.withSizeKeepingCentre (r.getWidth(), labelHeight + dial + textBoxHeight);

        label.setBounds  (group.removeFromTop (labelHeight));
        slider.setBounds (group);
    }

    juce::Slider& getSlider() { return slider; }

private:
    
    static constexpr int labelHeight = 20;
    
    juce::Slider slider;
    juce::Label  label;

    // L'attachment va SEMPRE dichiarato dopo lo slider: viene distrutto prima.
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;
    
    const int textBoxHeight;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LabeledKnob)
};
