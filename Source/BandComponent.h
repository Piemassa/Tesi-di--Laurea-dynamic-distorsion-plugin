#pragma once

#include <JuceHeader.h>
#include "LabeledKnob.h"
#include "PluginParameters.h"

typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

class BandComponent : public juce::Component
{
    public:
        BandComponent (juce::AudioProcessorValueTreeState& apvts, int bandIndex);
        ~BandComponent();
     
        void paint (juce::Graphics&) override;
        void resized() override;
     
    private:
        const int band;
     
        LabeledKnob driveKnob;
        LabeledKnob attackKnob;
        LabeledKnob sustainKnob;
    
        juce::TextButton driveON,transientON;
    
    
        std::unique_ptr<ButtonAttachment> driveByAttachment;
        std::unique_ptr<ButtonAttachment> transientByAttachment;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandComponent);
};

