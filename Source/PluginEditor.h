
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

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
    
    Slider driveAmountSlider;
    Slider typeSelectorSlider;
    Slider attackKnob;
    Slider sustainKnob;

    //gli attachment vanno dichiarati per ultimi altrimenti in modo che vengano distrutti prima degli slider
    std::unique_ptr<SliderAttachment> driveAmountAttachment;
    std::unique_ptr<SliderAttachment> typeSelectorAttachment;
    std::unique_ptr<SliderAttachment> attackKnobAttachment;
    std::unique_ptr<SliderAttachment> sustainKnobAttachment;
    
    juce::Label attackLabel;
    juce::Label sustainLabel;
    

    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Primo_distoAudioProcessorEditor)
};
