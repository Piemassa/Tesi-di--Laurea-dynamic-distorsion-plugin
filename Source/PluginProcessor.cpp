 
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Primo_distoAudioProcessor::Primo_distoAudioProcessor()

: AudioProcessor (BusesProperties() .withInput  ("Input",  juce::AudioChannelSet::stereo(), true) .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
parameters(*this, nullptr, "DIST", Parameters::createParameterLayout()),
m_Oversampler(2,2, juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR)

{
    
    Parameters::addListenerToAllParameters(parameters, this);
    
    distorsione_low.writeDistType(1);
    distorsione_mid.writeDistType(1);
    distorsione_high.writeDistType(1);
    
    driveCircuitState_low = 1;
    driveCircuitState_mid = 1;
    driveCircuitState_high = 1;
    
    transientCircuitState_low = 1;
    transientCircuitState_mid = 1;
    transientCircuitState_high = 1;
    
}

Primo_distoAudioProcessor::~Primo_distoAudioProcessor()
{
}

//==============================================================================
void Primo_distoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels      = static_cast<juce::uint32> (getTotalNumInputChannels());
    
    //inizializzazione filtri
    lowCrossoverLowPass.prepare(spec);
    lowCrossoverHighPass.prepare(spec);
    highCrossoverLowPass.prepare(spec);
    highCrossoverHighPass.prepare(spec);
    
    lowCrossoverLowPass.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    lowCrossoverHighPass.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    highCrossoverLowPass.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    highCrossoverHighPass.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    
    //frequenza di crossover (da cambaire successivamente con dei parametri)
    
    lowFreq = 250.0f;
    highFreq = 2500.0f;
    
    lowCrossoverLowPass.setCutoffFrequency(lowFreq);
    lowCrossoverHighPass.setCutoffFrequency(lowFreq);
    
    highCrossoverLowPass.setCutoffFrequency(highFreq);
    highCrossoverHighPass.setCutoffFrequency(highFreq);
    
    lowBuffer.setSize(getTotalNumInputChannels(), samplesPerBlock);
    midBuffer.setSize(getTotalNumInputChannels(), samplesPerBlock);
    highBuffer.setSize(getTotalNumInputChannels(), samplesPerBlock);
    
    
    
    meterInput.prepareToPlay(sampleRate);
    
    m_Oversampler.reset();
    m_Oversampler.initProcessing(samplesPerBlock);
    
    transientShaper_low.prepareToPlay(sampleRate, samplesPerBlock);
    transientShaper_mid.prepareToPlay(sampleRate, samplesPerBlock);
    transientShaper_high.prepareToPlay(sampleRate, samplesPerBlock);
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
    
    //meterINPUT
    meterInput.processBlock(buffer, numSample, inCh, outCh);
    
    
    
    // --------------------- multibanda -------------- //
    
    for(int ch = 0; ch < inCh; ++ch)
    {
        lowBuffer.copyFrom(ch, 0, buffer, ch, 0, numSample);
        midBuffer.copyFrom(ch, 0, buffer, ch, 0, numSample);
    }
    
    juce::dsp::AudioBlock<float> lowBlock (lowBuffer);
    juce::dsp::AudioBlock<float> midBlock (midBuffer);
    
    lowCrossoverLowPass.process (juce::dsp::ProcessContextReplacing<float>(lowBlock));
    lowCrossoverHighPass.process (juce::dsp::ProcessContextReplacing<float>(midBlock));
    
    //secondo split
    
    for (int ch = 0; ch < inCh; ++ch)
    {
        highBuffer.copyFrom( ch, 0, midBuffer, ch, 0, numSample);
    }
    
    juce::dsp::AudioBlock<float> highBlock (highBuffer);
    
    highCrossoverLowPass.process (juce::dsp::ProcessContextReplacing<float>(midBlock));
    highCrossoverHighPass.process (juce::dsp::ProcessContextReplacing<float>(highBlock));
    
    auto leftChannel_low = lowBuffer.getWritePointer(0);
    auto rightChannel_low = lowBuffer.getWritePointer(1);
    
    auto leftChannel_mid= midBuffer.getWritePointer(0);
    auto rightChannel_mid = midBuffer.getWritePointer(1);
    
    auto leftChannel_high = highBuffer.getWritePointer(0);
    auto rightChannel_high = highBuffer.getWritePointer(1);
    
    // --------------------- multibanda -------------- //
    
    

    //NUOVO PEZZO DI PLUGIN
    
    
    for(AudioModuleType module : moduleChain)
    {
        switch (module)
        {
            case AudioModuleType::TransientShaperModule:
                
                //LOW TRANSIENT
                if(transientCircuitState_low == 1)
                {
                    for(int smp = 0; smp < numSample; ++smp )
                    {
                        
                        float leftSampleCompare = std::abs(lowBuffer.getSample(0, smp));
                        float rightSampleCompare = std::abs(lowBuffer.getSample(1, smp));
                        
                        //lo rende mono ovvero il sample su cui lavoro sarà il max tra sinistra e destra
                        float input_shaper = std::max(leftSampleCompare,rightSampleCompare);
                        
                        //chiamo transient shaper process block e gli passo il sample di input
                        transientShaper_low.processBlock(input_shaper);
                        
                        //userò un metodo di transient shaper per farmi tornare il valore di gain da applicare al sample
                        float gainMod = transientShaper_low.getGainValue();
                        
                        //applico il valore di gain al sample
                        leftChannel_low[smp] *= gainMod;
                        rightChannel_low[smp] *= gainMod;

                    }
                }
                
                //MID TRANSIENT
                if(transientCircuitState_mid == 1)
                {
                    for(int smp = 0; smp < numSample; ++smp )
                    {
                        
                        float leftSampleCompare = std::abs(midBuffer.getSample(0, smp));
                        float rightSampleCompare = std::abs(midBuffer.getSample(1, smp));
                        
                        //lo rende mono ovvero il sample su cui lavoro sarà il max tra sinistra e destra
                        float input_shaper = std::max(leftSampleCompare,rightSampleCompare);
                        
                        //chiamo transient shaper process block e gli passo il sample di input
                        transientShaper_mid.processBlock(input_shaper);
                        
                        //userò un metodo di transient shaper per farmi tornare il valore di gain da applicare al sample
                        float gainMod = transientShaper_mid.getGainValue();
                        
                        //applico il valore di gain al sample
                        leftChannel_mid[smp] *= gainMod;
                        rightChannel_mid[smp] *= gainMod;
                        
                    }
                }
                
                //HIGH TRANSIENT
                if(transientCircuitState_high == 1)
                {
                    for(int smp = 0; smp < numSample; ++smp )
                    {
                        
                        float leftSampleCompare = std::abs(highBuffer.getSample(0, smp));
                        float rightSampleCompare = std::abs(highBuffer.getSample(1, smp));
                        
                        //lo rende mono ovvero il sample su cui lavoro sarà il max tra sinistra e destra
                        float input_shaper = std::max(leftSampleCompare,rightSampleCompare);
                        
                        //chiamo transient shaper process block e gli passo il sample di input
                        transientShaper_high.processBlock(input_shaper);
                        
                        //userò un metodo di transient shaper per farmi tornare il valore di gain da applicare al sample
                        float gainMod = transientShaper_high.getGainValue();
                        
                        //applico il valore di gain al sample
                        leftChannel_high[smp] *= gainMod;
                        rightChannel_high[smp] *= gainMod;
                        
                    }
                }
                
                break;
                
            case AudioModuleType::DistorsionModule:
                
                //LOW DISTORSION
                if(driveCircuitState_low == 1)
                {
                    auto block =  juce::dsp::AudioBlock<float>(lowBuffer);
                    auto upBlock = m_Oversampler.processSamplesUp(block);
                    
                    for (int ch = 0; ch < upBlock.getNumChannels(); ++ch)
                    {
                        
                        auto* channelWriterPointer = upBlock.getChannelPointer(ch);
                        
                        for (int smp = 0; smp < upBlock.getNumSamples(); ++smp)
                        {
                            distorsione_low.processBlock(channelWriterPointer[smp]);
                        }
                    }
                    
                    m_Oversampler.processSamplesDown(block);
                }
                
                //MID DISTORSION
                if(driveCircuitState_mid == 1)
                {
                    auto block =  juce::dsp::AudioBlock<float>(midBuffer);
                    auto upBlock = m_Oversampler.processSamplesUp(block);
                    
                    for (int ch = 0; ch < upBlock.getNumChannels(); ++ch)
                    {
                        
                        auto* channelWriterPointer = upBlock.getChannelPointer(ch);
                        
                        for (int smp = 0; smp < upBlock.getNumSamples(); ++smp)
                        {
                            distorsione_mid.processBlock(channelWriterPointer[smp]);
                        }
                    }
                    
                    m_Oversampler.processSamplesDown(block);
                }
                
                //HIGH DISTORSION
                if(driveCircuitState_high == 1)
                {
                    auto block =  juce::dsp::AudioBlock<float>(highBuffer);
                    auto upBlock = m_Oversampler.processSamplesUp(block);
                    
                    for (int ch = 0; ch < upBlock.getNumChannels(); ++ch)
                    {
                        
                        auto* channelWriterPointer = upBlock.getChannelPointer(ch);
                        
                        for (int smp = 0; smp < upBlock.getNumSamples(); ++smp)
                        {
                            distorsione_high.processBlock(channelWriterPointer[smp]);
                        }
                    }
                    
                    m_Oversampler.processSamplesDown(block);
                }
                
            default:
                break;
        }
    }
    
    // --------------------- multibanda -------------- //
    
    buffer.clear(); // Pulisce il buffer di output
        for (int ch = 0; ch < inCh; ++ch)
        {
            buffer.addFrom (ch, 0, lowBuffer,  ch, 0, numSample);
            buffer.addFrom (ch, 0, midBuffer,  ch, 0, numSample);
            buffer.addFrom (ch, 0, highBuffer, ch, 0, numSample);
        }
    
    // --------------------- multibanda -------------- //
    
    
    //meterOut
    meterOutput.processBlock(buffer, numSample, inCh, outCh);
    
}

void Primo_distoAudioProcessor::parameterChanged(const String& paramID, float newValue)
{
    
    
    if(paramID == Parameters::nameDriveLow)
    {
        /*
         il valore di driveamount si deve aggirare da 0 a 5. Il valore che ricevo dal parametro è una percentuale.
         quello che ho bisogno di fare prima di poter mettere il valore all'interno di DriveAmount è rimapparlo in modo
         che sia espresso da 0 a 5. Passaggio fondamentale aggiungerei.
         Per farlo prendo ogni valore del parametro (che vanno da 0 a 100) e lo moltiplico per 0.05
         Dopodichè sommo + 1 in modo da non avere dei valori di driveamount < 1
        */
        distorsione_low.write_DriveAmount(newValue * 0.05f);
    }
    
    if(paramID == Parameters::nameDriveMid)
    {
        distorsione_mid.write_DriveAmount(newValue * 0.05f);
    }
    
    if(paramID == Parameters::nameDriveHigh)
    {
        distorsione_high.write_DriveAmount(newValue * 0.05f);
    }
    
    if(paramID == Parameters::nameMode)
    {
        distorsione_low.writeDistType(newValue);
        distorsione_mid.writeDistType(newValue);
        distorsione_high.writeDistType(newValue);
    }
    
    if(paramID == Parameters::nameAttackLow)
    {
        transientShaper_low.writeAttackKnob(newValue);
    }
    if(paramID == Parameters::nameAttackMid)
    {
        transientShaper_mid.writeAttackKnob(newValue);
    }
    if(paramID == Parameters::nameAttackHigh)
    {
        transientShaper_high.writeAttackKnob(newValue);
    }
    
    if(paramID == Parameters::nameSustainLow)
    {
        transientShaper_low.writeSustainKnob(newValue);
    }
    if(paramID == Parameters::nameSustainMid)
    {
        transientShaper_mid.writeSustainKnob(newValue);
    }
    if(paramID == Parameters::nameSustainHigh)
    {
        transientShaper_high.writeSustainKnob(newValue);
    }
    
    if(paramID == Parameters::nameDriveBypassLow)
    {
        driveCircuitState_low = newValue;
    }
    
    if(paramID == Parameters::nameDriveBypassMid)
    {
        driveCircuitState_mid = newValue;
    }
    
    if(paramID == Parameters::nameDriveBypassHigh)
    {
        driveCircuitState_high = newValue;
    }
    
    if(paramID == Parameters::nameTransientBypassLow)
    {
        transientCircuitState_low = newValue;
    }
    
    if(paramID == Parameters::nameTransientBypassMid)
    {
        transientCircuitState_mid = newValue;
    }
    
    if(paramID == Parameters::nameTransientBypassHigh)
    {
        transientCircuitState_high = newValue;
    }
    
    if(paramID == Parameters::nameOrder)
    {
        updateModuleOrder(newValue);
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
    
    if(orderChoice == 1)
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

bool Primo_distoAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet()  == juce::AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

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

