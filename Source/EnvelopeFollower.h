#pragma once
#include <JuceHeader.h>


class Envelope_Follower {

    
    public:
    
    Envelope_Follower();
    ~Envelope_Follower();
    
    void prepareToPlay(double newSampleRate, int samplesPerBlock);
    void processBlock(float SampleToProcess); 
    
    void reset();
    
    void setAttackTime(float attackMs);
    void setReleaseTime(float releaseMs);
    
    float getCurrentEnvelope();
    
    private:
    
//    void updateCurrentEnvelope(float newValue);
    
    void updateAttackCoeff();
    
    void updateReleaseCoeff();
    
    double sampleRate = 0.0f;
    float attackTimeMs = 1.0f;
    float releaseTimeMs = 100.0f;

    float attackCoef = 0.0f;
    float releaseCoef = 0.0f;

    float currentEnvelope = 0.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Envelope_Follower);
};
