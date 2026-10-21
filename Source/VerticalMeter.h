
#pragma once

#include <JuceHeader.h>
#include "MeteringProcess.h"
#include "PeakMeter.h"
#include "ExtraDef.h"

//CONTENUTO DI EXTRADEF PER QUESTA CLASSE
//#define RT 0.6f
//#define DB_FLOOR -70.0f
//#define DB_LIMIT 6.0f
//#define FPS 30
//#define DB_VALUES_MARK

//==============================================================================

class VerticalMeter  : public juce::Component, public Timer
{
public:
    VerticalMeter(MeterPersonale::MeteringDsp& linkRef);
    ~VerticalMeter() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;
    

    
private:
    
    PeakMeter leftPeak,rightPeak;

    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VerticalMeter)
};




//ATTENTION when setting bounds for this type of object set your personal width
//then add 20 extra and then 1 extra for a perfect fitting of all meters and lebels

