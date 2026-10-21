#pragma once

#include <JuceHeader.h>

namespace MeterPersonale {

class MeteringDsp
{
public:
    
    MeteringDsp();
    ~MeteringDsp();
    
    void prepareToPlay(double sampleRate);
    
    void processBlock(AudioBuffer<float>& buffer, const int numSamples, const int inCh, const int outCh );
    
    float getRmsLevelSmooth(const int channel) const; // sistemare
    
    Atomic<float>& getPeakVariable(const int channel);
    
    void setPeakLevel(const float value, const int channel);
    
    
    
private:
    
    void smoothValueRamp(float value, LinearSmoothedValue<float>&);
    
    void setStartingValues();
    
    LinearSmoothedValue<float> rmsLevelLeft, rmsLevelRight;
    Atomic<float> peakLeft, peakRight;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MeteringDsp)
};


}



// i valori a cui si accede da questa classe sono sempre in dB
