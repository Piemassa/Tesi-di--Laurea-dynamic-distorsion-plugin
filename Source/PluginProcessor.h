#pragma once

#include <JuceHeader.h>
#include "Distorsori.h"
#include <juce_dsp/juce_dsp.h>
#include "TransientShaper.h"
#include "MeteringProcess.h"
#include "PluginParameters.h"

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
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    //==============================================================================
    int getNumPrograms() override { return 1;}
    int getCurrentProgram() override {return 0;}
    void setCurrentProgram (int index) override {}
    const juce::String getProgramName (int index) override {return {};}
    void changeProgramName (int index, const juce::String& newName) override {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    
    //void updateModule
    
    enum class AudioModuleType {
        TransientShaperModule,
        DistorsionModule
    };
    
    MeterPersonale::MeteringDsp& linkMeter(int meterNumber);
    
    AudioProcessorValueTreeState parameters;
    

private:
    
    //vettore moduli
    std::vector<AudioModuleType> moduleChain {
        AudioModuleType::TransientShaperModule, AudioModuleType::DistorsionModule
    };
    
    Distorsion_P distorsione_low, distorsione_mid, distorsione_high;
    
    Transient_Shaper transientShaper_low, transientShaper_mid, transientShaper_high;
    
    //Crossover 1 Low
    juce::dsp::LinkwitzRileyFilter<float> lowCrossoverLowPass;
    juce::dsp::LinkwitzRileyFilter<float> lowCrossoverHighPass;
    
    //Crossover 2 Low
    juce::dsp::LinkwitzRileyFilter<float> highCrossoverLowPass;
    juce::dsp::LinkwitzRileyFilter<float> highCrossoverHighPass;
    
    //bufferseparati per le 3 bande
    
    juce::AudioBuffer<float> lowBuffer;
    juce::AudioBuffer<float> midBuffer;
    juce::AudioBuffer<float> highBuffer;
    
    //frequenze di crossover
    
    float lowFreq, highFreq;
    
    
    juce::dsp::Oversampling<float>  m_Oversampler;
    void parameterChanged(const String& paramID, float newValue) override;
    
    void updateModuleOrder(int orderChoice);
    
    MeterPersonale::MeteringDsp meterInput;
    MeterPersonale::MeteringDsp meterOutput;
    
    bool driveCircuitState_low, transientCircuitState_low;
    bool driveCircuitState_mid, transientCircuitState_mid;
    bool driveCircuitState_high, transientCircuitState_high;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Primo_distoAudioProcessor)
};
