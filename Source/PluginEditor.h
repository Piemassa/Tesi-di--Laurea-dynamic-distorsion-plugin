
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "VerticalMeter.h"
#include "BandComponent.h"

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
    
    std::unique_ptr<BandComponent> bandLow, bandMid, bandHigh;

    juce::TextButton driveButton[5];
    juce::ComboBox moduleOrder;
    

    //gli attachment vanno dichiarati per ultimi in modo che vengano distrutti prima degli slider
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> moduleOrderAttachment;
    
    
    std::unique_ptr<juce::ParameterAttachment> modeAttachment; //attachment per la selezione della modalità di distorsione
    

    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Primo_distoAudioProcessorEditor)
};
