
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "VerticalMeter.h"

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

//==============================================================================

class Primo_distoAudioProcessorEditor  : public juce::AudioProcessorEditor 
{
public:
    Primo_distoAudioProcessorEditor (Primo_distoAudioProcessor&, AudioProcessorValueTreeState& );
    ~Primo_distoAudioProcessorEditor() override;

    
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    Primo_distoAudioProcessor& audioProcessor;
    AudioProcessorValueTreeState& parameters;
    
    void updateModeButtons (float newValue);
    
    VerticalMeter meterIn, meterOut;
    
    Slider driveAmountKnob;
//    Slider typeSelectorSlider;
    Slider attackKnob;
    Slider sustainKnob;

    juce::TextButton driveButton[5];
    juce::TextButton driveON,transientON;
    

    //gli attachment vanno dichiarati per ultimi altrimenti in modo che vengano distrutti prima degli slider
    std::unique_ptr<SliderAttachment> driveAmountAttachment;
//    std::unique_ptr<SliderAttachment> typeSelectorAttachment;
    std::unique_ptr<SliderAttachment> attackKnobAttachment;
    std::unique_ptr<SliderAttachment> sustainKnobAttachment;
    std::unique_ptr<ButtonAttachment> driveByAttachment;
    std::unique_ptr<ButtonAttachment> transientByAttachment;
    
    
    std::unique_ptr<juce::ParameterAttachment> modeAttachment;
    
    juce::Label attackLabel;
    juce::Label sustainLabel;
    

    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Primo_distoAudioProcessorEditor)
};
