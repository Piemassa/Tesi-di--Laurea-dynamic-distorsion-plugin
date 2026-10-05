 
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Primo_distoAudioProcessor::Primo_distoAudioProcessor()

: parameters(*this, nullptr, "DIST",
                {
                    std::make_unique<juce::AudioParameterFloat>(ParameterID("DRIVEAMOUNT", 1), "Drive (%)", NormalisableRange<float>( 0.0f, 100.0f, 0.01f), 0.0f ),
                    std::make_unique<juce::AudioParameterFloat>(ParameterID("TYPE", 1), "Type", NormalisableRange<float>(0.0f, 6.0f, 1.0f), 0.0f ),
                    std::make_unique<juce::AudioParameterFloat>(ParameterID("ATTACK", 1), "Attack", NormalisableRange<float>( -1.0f, 1.0f, 0.001f), 0.0f),
                    std::make_unique<juce::AudioParameterFloat>(ParameterID("SUSTAIN", 1), "Sustain", NormalisableRange<float>( -1.0f, 1.0f, 0.001f), 0.0f),
            
                }
             ), m_Oversampler(2,2, juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR)

{
    
    parameters.addParameterListener("DRIVEAMOUNT", this);
    parameters.addParameterListener("TYPE", this);
    parameters.addParameterListener("ATTACK", this);
    parameters.addParameterListener("SUSTAIN", this);
    
}

Primo_distoAudioProcessor::~Primo_distoAudioProcessor()
{
}

//==============================================================================
void Primo_distoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    m_Oversampler.reset();
    m_Oversampler.initProcessing(samplesPerBlock);
    
    TransientShaper.prepareToPlay(sampleRate, samplesPerBlock);
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
    
    auto leftChannel = buffer.getWritePointer(0);
    auto rightChannel = buffer.getWritePointer(1);
    
    //transient shaper
    
    for(int smp = 0; smp < numSample; ++smp )
    {
        
        float leftSampleCompare = std::abs(buffer.getSample(0, smp));
        float rightSampleCompare = std::abs(buffer.getSample(1, smp));
        
        //lo rende mono ovvero il sample su cui lavoro sarà il max tra sinistra e destra
        float input_shaper = std::max(leftSampleCompare,rightSampleCompare);
        
        //chiamo transient shaper process block e gli passo il sample di input
        TransientShaper.processBlock(input_shaper);
        
        //userò un metodo di transient shaper per farmi tornare il valore di gain da applicare al sample
        float gainMod = TransientShaper.getGainValue();
        
        //applico il valore di gain al sample
        leftChannel[smp] *= gainMod;
        rightChannel[smp] *= gainMod;
        //buffer.setSample(0, smp, buffer.getSample(0, smp) * gainMod); //modulo il sample left channel
        //buffer.setSample(1, smp, buffer.getSample(1, smp) * gainMod); //modulo il sample right channel
        
        
    }
    
    
    //distorsione
    
    auto block =  juce::dsp::AudioBlock<float>(buffer);
    auto upBlock = m_Oversampler.processSamplesUp(block);

    for (int ch = 0; ch < upBlock.getNumChannels(); ++ch)
    {
        
        auto* channelWriterPointer = upBlock.getChannelPointer(ch);
        
        for (int smp = 0; smp < upBlock.getNumSamples(); ++smp)
        {
            distorsione.processBlock(channelWriterPointer[smp]);
        }
    }
    
    m_Oversampler.processSamplesDown(block);
    
}

void Primo_distoAudioProcessor::parameterChanged(const String& paramID, float newValue)
{
    
    
    if(paramID == "DRIVEAMOUNT")
    {
        /*
         il valore di driveamount si deve aggirare da 0 a 5. Il valore che ricevo dal parametro è una percentuale.
         quello che ho bisogno di fare prima di poter mettere il valore all'interno di DriveAmount è rimapparlo in modo
         che sia espresso da 0 a 5. Passaggio fondamentale aggiungerei.
         Per farlo prendo ogni valore del parametro (che vanno da 0 a 100) e lo moltiplico per 0.05
        */
        distorsione.write_DriveAmount(newValue * 0.05f);
    }
    
    if(paramID == "TYPE")
    {
        distorsione.writeDistType(newValue);
    }
    
    if(paramID == "ATTACK")
    {
        TransientShaper.writeAttackKnob(newValue);
    }
    
    if(paramID == "SUSTAIN")
    {
        TransientShaper.writeSustainKnob(newValue);
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

