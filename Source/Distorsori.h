#pragma once
#include <JuceHeader.h>
#include "math.h"

class Distorsion_P {
    
    public:
    
    Distorsion_P();
    ~Distorsion_P();
    
    void prepareToPlay();
    void processBlock(float &SampleToProcess);
    
    int readDistType(); //ritorna il valore del parametro che sceglie il tipo di distorsione
    void writeDistType(int newValue);
    
    float read_DriveAmount();
    
    void write_DriveAmount(float newValue);
    
    private:
    
    int dist_type = 0;
    juce::Atomic<float> driveAmount = 0.0;
    
    void bypassDistorsion();                              //tipo 0
    void Tanh(float &sample, float driveAmount );         //tipo 1 bella ma centra poco con quello che devo fare io
    void softSign(float &sample, float driveAmount);      //tipo 2 bella ma centra poco con quello che devo fare io
    void TapeLike2(float &sample, float driveAmount);     //tipo 3 bella ma centra poco con quello che devo fare io
    void disto4(float &sample, float driveAmount);        //tipo 4 approvata
    void turboDistorto(float &sample, float driveAmount); //tipo 5 pirkle distortion bella ma centra poco con quello che devo fare io
    void wavefolding(float &sample, float driveAmount);
    
    
    // Funzione base di Hermite per t in [0, 1]
    float evalHermite(float t, float p0, float p1, float m0, float m1);
    
    float processSampleHermite(float x);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Distorsion_P);
};
