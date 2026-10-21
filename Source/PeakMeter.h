#pragma once

#include <JuceHeader.h>
#include "ExtraDef.h"



class PeakMeter  : public juce::Component
{
public:
    PeakMeter();
    ~PeakMeter() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void connectTo(Atomic<float>& signalSnapshot);

private:
    
    Atomic<float>* probedSignal = nullptr;
    
    float alpha = 0.0f;
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PeakMeter)
};
