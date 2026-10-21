#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class RmsMeter  : public juce::Component
{
public:
    RmsMeter();
    ~RmsMeter() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RmsMeter)
};
