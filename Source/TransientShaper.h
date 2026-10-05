#pragma once
#include <JuceHeader.h>
#include "EnvelopeFollower.h"

class Transient_Shaper {
    
    public:
    
    Transient_Shaper();
    ~Transient_Shaper();
    
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void processBlock(float workingSample);
    
    float getGainValue();
    
    void writeAttackKnob(float newValue);
    float readAttackKnob();
    
    void writeSustainKnob(float newValue);
    float readSustainKnob();
    
    
    private:
    
    void setGainModulation(float newValue);
    
    Envelope_Follower fastEnvelope;
    Envelope_Follower slowEnvelope;
    
    float attackDifferential = 0.0f;
    float sustainDifferential = 0.0f;
    
    //Questi vanno collegati ai parametri
    float attackKnob = 0.0f, sustainKnob = 0.0f;
    
    float gainModulation = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Transient_Shaper);
};
