#pragma once

#include <JuceHeader.h>
#include "Distorsori.h"
#include <juce_dsp/juce_dsp.h>
#include "TransientShaper.h"
#include "MeteringProcess.h"

//==============================================================================
class Primo_distoAudioProcessor  : public juce::AudioProcessor, public AudioProcessorValueTreeState::Listener
{
    public:
    //==============================================================================
    Primo_distoAudioProcessor();
    ~Primo_distoAudioProcessor() override;
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    
    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override {return true;}

    //==============================================================================
    const juce::String getName() const override {return JucePlugin_Name;}

    bool acceptsMidi() const override {return false;}
    bool producesMidi() const override {return false;}
    bool isMidiEffect() const override {return false;}
    double getTailLengthSeconds() const override {return 0.0;}

    //==============================================================================
    int getNumPrograms() override { return 1;}
    int getCurrentProgram() override {return 0;}
    void setCurrentProgram (int index) override {}
    const juce::String getProgramName (int index) override {return {};}
    void changeProgramName (int index, const juce::String& newName) override {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    MeterPersonale::MeteringDsp& linkMeter(int meterNumber);
    
    AudioProcessorValueTreeState parameters;
    

private:
    
    Distorsion_P distorsione;
    
    Transient_Shaper TransientShaper;
    
    
    juce::dsp::Oversampling<float>  m_Oversampler;
    void parameterChanged(const String& paramID, float newValue) override;
    
    MeterPersonale::MeteringDsp meterInput;
    MeterPersonale::MeteringDsp meterOutput;
    
    bool driveCircuitState, transientCircuitState;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Primo_distoAudioProcessor)
};
