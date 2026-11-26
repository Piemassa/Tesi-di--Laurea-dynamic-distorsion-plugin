 
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Primo_distoAudioProcessor::Primo_distoAudioProcessor()

: parameters(*this, nullptr, "DIST", Parameters::createParameterLayout()),
m_Oversampler(2,2, juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR)

{
    
    Parameters::addListenerToAllParameters(parameters, this);
    
    distorsione.writeDistType(1);
    driveCircuitState = 1;
    transientCircuitState = 1;
    
}

Primo_distoAudioProcessor::~Primo_distoAudioProcessor()
{
}

//==============================================================================
void Primo_distoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    meterInput.prepareToPlay(sampleRate);
    
    const int    factor  = (int) m_Oversampler.getOversamplingFactor();   // 4 con Oversampling (2, 2, ...)
    const double osRate  = sampleRate * factor;
    const int    osBlock = samplesPerBlock * factor;
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = osRate;
    spec.maximumBlockSize = (juce::uint32) osBlock;
    spec.numChannels      = 2;
    
    m_Oversampler.reset();
    m_Oversampler.initProcessing(samplesPerBlock);
    
    smoothInput.reset(sampleRate, 0.025f);
    smoothInput.setCurrentAndTargetValue(1.0f);
    smoothOutput.reset(sampleRate, 0.025f);
    smoothOutput.setCurrentAndTargetValue(1.0f);
    
    
    
    TransientShaper.prepareToPlay(osRate, osBlock);
}

void Primo_distoAudioProcessor::releaseResources()
{

}

void Primo_distoAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto inCh  = getTotalNumInputChannels();
    auto outCh = getTotalNumOutputChannels();
    auto numSample = buffer.getNumSamples();
    bool stereo = buffer.getNumChannels() > 1 ? true : false;
    
    smoothInput.applyGain(buffer, numSample);
    
    //meterINPUT
    meterInput.processBlock(buffer, numSample, inCh, outCh);
    
    //UPSAMPLE
    auto block =  juce::dsp::AudioBlock<float>(buffer);
    auto upBlock = m_Oversampler.processSamplesUp(block);
    
    auto leftChannel = upBlock.getChannelPointer(0);
    auto rightChannel = stereo ?  upBlock.getChannelPointer(1) : nullptr;

    
    
    
    //ciclo su tutti i sample
    for(size_t smp = 0; smp < upBlock.getNumSamples(); ++smp)
    {
        float leftSampleCompare = std::abs(leftChannel[smp]);
        float rightSampleCompare = stereo ? std::abs(rightChannel[smp]) : leftSampleCompare;
        
        float input_shaper = std::max(leftSampleCompare, rightSampleCompare);
        TransientShaper.processBlock(input_shaper);
        
        float gainMod = TransientShaper.getGainValue();
        
        //ciclo sui moduli
        for(AudioModuleType module : moduleChain)
        {
            //switch per ordine dei moduli
            switch (module)
            {
                case AudioModuleType::TransientShaperModule:
                    
                    if(transientCircuitState == 1)
                    {
                        leftChannel[smp] *= gainMod;
                        if(stereo)
                        {
                            rightChannel[smp] *= gainMod;
                        }
                    }
                    break;
                    
                case AudioModuleType::DistorsionModule:
                    
                    if(driveCircuitState == 1)
                    {
                        distorsione.processBlock(leftChannel[smp]);
                        if(stereo)
                        {
                            distorsione.processBlock(rightChannel[smp]);
                        }
                    }
                    
                default:
                break;
            }
        }
        
        
        
    }
 
    //DOWNSAMPLE
    m_Oversampler.processSamplesDown(block);
    
    smoothOutput.applyGain(buffer, numSample);
    
    //meterOut
    meterOutput.processBlock(buffer, numSample, inCh, outCh);
    
}

void Primo_distoAudioProcessor::parameterChanged(const String& paramID, float newValue)
{
    
    
    if(paramID == Parameters::nameDrive)
    {
        /*
         il valore di driveamount si deve aggirare da 0 a 5. Il valore che ricevo dal parametro è una percentuale.
         quello che ho bisogno di fare prima di poter mettere il valore all'interno di DriveAmount è rimapparlo in modo
         che sia espresso da 0 a 5. Passaggio fondamentale aggiungerei.
         Per farlo prendo ogni valore del parametro (che vanno da 0 a 100) e lo moltiplico per 0.05
         Dopodichè sommo + 1 in modo da non avere dei valori di driveamount < 1
        */
        distorsione.write_DriveAmount(newValue * 0.05f);
    }
    
    if(paramID == Parameters::nameMode)
    {
        distorsione.writeDistType(newValue);
    }
    
    if(paramID == Parameters::nameAttack)
    {
        TransientShaper.writeAttackKnob(newValue);
    }
    
    if(paramID == Parameters::nameSustain)
    {
        TransientShaper.writeSustainKnob(newValue);
    }
    
    if(paramID == Parameters::nameDriveBypass)
    {
        driveCircuitState = newValue;
    }
    
    if(paramID == Parameters::nameTransientBypass)
    {
        transientCircuitState = newValue;
    }
    
    if(paramID == Parameters::nameOrder)
    {
        updateModuleOrder(newValue);
    }
    
    if(paramID == Parameters::nameGainIn)
    {
        smoothInput.setTargetValue(juce::Decibels::decibelsToGain(newValue));
    }
    if(paramID == Parameters::nameGainOut)
    {
        smoothOutput.setTargetValue(juce::Decibels::decibelsToGain(newValue));
    }
    
}


MeterPersonale::MeteringDsp& Primo_distoAudioProcessor::linkMeter(int meterNumber)
{
    switch (meterNumber) {
        case 0:
            return meterInput;
            break;
        case 1:
            return meterOutput;
        default:
            break;
    }
    return meterInput;
}

void Primo_distoAudioProcessor::updateModuleOrder(int orderChoice)
{
    // 0 = transientShaper -> distorsion
    // 1 = distorsion -> transientShaper
    moduleChain.clear();
    
    if(orderChoice == 0)
    {
        moduleChain.push_back(AudioModuleType::TransientShaperModule);
        moduleChain.push_back(AudioModuleType::DistorsionModule);
    }
    else
    {
        moduleChain.push_back(AudioModuleType::DistorsionModule);
        moduleChain.push_back(AudioModuleType::TransientShaperModule);
    }
}

//==============================================================================

juce::AudioProcessorEditor* Primo_distoAudioProcessor::createEditor()
{
    return new Primo_distoAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void Primo_distoAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void Primo_distoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Primo_distoAudioProcessor();
}

