#include "MeteringProcess.h"


MeterPersonale::MeteringDsp::MeteringDsp()
{
    setStartingValues();
}
MeterPersonale::MeteringDsp::~MeteringDsp()
{
    
}

void MeterPersonale::MeteringDsp::prepareToPlay(double sampleRate)
{
    //rms settings
    rmsLevelLeft.reset(sampleRate, 0.18f);
    rmsLevelRight.reset(sampleRate, 0.18f);
    
    rmsLevelLeft.setCurrentAndTargetValue(-100.0f);
    rmsLevelRight.setCurrentAndTargetValue(-100.0f);
    
    //peak settings
    peakLeft.set(-100.0f);
    peakRight.set(-100.0f);
    
}

void MeterPersonale::MeteringDsp::processBlock(AudioBuffer<float>& buffer, const int numSamples, const int inCh, const int outCh )
{
    
    if(inCh < outCh) //{1,2}
    {
        buffer.copyFrom(1, 0, buffer, 0, 0, numSamples); // trasformo caso {1,2} in {2,2}
    }
    
    
    if( (inCh + outCh) == 2) // caso {1,1} ovvero calcolo rms se sono in mono
    {
        
        auto rawRMSMono = Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, numSamples)); //mi calcolo rms del segnale mono
        
        //inserisco i miei valori ottenuti per essere smussati come voglio io
        smoothValueRamp(rawRMSMono, rmsLevelLeft);
        
        //copio il valore dei rms smoothed del canale sinistro nel canale destro in modo che entrambe le barre del meter si muovano
        rmsLevelRight = rmsLevelLeft;
        
        
        //itero sui valori smussati
        for (int s = 0; s < numSamples; ++s)
        {
            rmsLevelLeft.getNextValue();
            rmsLevelRight.getNextValue();
        }
        
        
        //calcolo picco canale mono
        float dbMagniMono = Decibels::gainToDecibels(buffer.getMagnitude(0, 0, numSamples));

        //setto il valore del picco mono su entrambi i meter
        setPeakLevel(dbMagniMono, 0);
        setPeakLevel(dbMagniMono, 1);

    }
    
    
    else //entro  nell'else in {2,2}{2,1}{1,2}
    {
        
        auto rawRMSL = Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, numSamples)); //getRMS canale sinistro e trasformo in db
        auto rawRMSR = Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, numSamples));; //getRMS canale destro e trasformo in db
        
        
        //inserisco i miei valori ottenuti per essere smussati come voglio io
        smoothValueRamp(rawRMSL, rmsLevelLeft); //canale sinistro
        smoothValueRamp(rawRMSR, rmsLevelRight); //canale destro
        
        
        //itero sui valori smussati
        for (int s = 0; s < numSamples; ++s)
        {
            rmsLevelLeft.getNextValue();
            rmsLevelRight.getNextValue();
        }
        
        //calcolo picco canale sinistro e destro
        float dbMagniL = Decibels::gainToDecibels(buffer.getMagnitude(0, 0, numSamples));
        float dbMagniR = Decibels::gainToDecibels(buffer.getMagnitude(1, 0, numSamples));
        
        //setto il valore del picco sinistro e destro
        setPeakLevel(dbMagniL, 0);
        setPeakLevel(dbMagniR, 1);
        
    }
    
    
    if(inCh > outCh) //caso {2,1}
    {
        //sommo il contenuto del canale 1 al canale 0 quindi faccio un mix mono di due canali stereo
        buffer.copyFrom(0, 0, buffer, 1, 0, numSamples);
        
        auto rawRMSMono = Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, numSamples)); //mi calcolo rms del segnale mono
        
        //inserisco i miei valori ottenuti per essere smussati come voglio io
        smoothValueRamp(rawRMSMono, rmsLevelLeft);
        
        //copio il valore dei rms smoothed del canale sinistro nel canale destro in modo che entrambe le barre del meter si muovano
        rmsLevelRight = rmsLevelLeft;
        
        
        //itero sui valori smussati
        for (int s = 0; s < numSamples; ++s)
        {
            rmsLevelLeft.getNextValue();
            rmsLevelRight.getNextValue();
        }
        
        
        //calcolo picco canale mono
        float dbMagniMono = Decibels::gainToDecibels(buffer.getMagnitude(0, 0, numSamples));

        //setto il valore del picco mono su entrambi i meter
        setPeakLevel(dbMagniMono, 0);
        setPeakLevel(dbMagniMono, 1);
        
    }
}

float MeterPersonale::MeteringDsp::getRmsLevelSmooth(const int channel) const
{
    jassert(channel == 0 || channel == 1);
    if(channel == 0)
    {
        return rmsLevelLeft.getCurrentValue();
    }
    if(channel == 1)
    {
        return rmsLevelRight.getCurrentValue();
    }
    
    return 0.0f;
}

Atomic<float>& MeterPersonale::MeteringDsp::getPeakVariable(const int channel)
{
    jassert(channel == 0 || channel == 1);
    if(channel == 0)
    {
        return peakLeft;
    }
    if(channel == 1)
    {
        return peakRight;
    }
    
    throw std::runtime_error("An error occurred in getPeak");
    
}

void MeterPersonale::MeteringDsp::setPeakLevel(const float value, const int channel)
{
    jassert(channel == 0 || channel == 1);
    if (channel == 0) //cambio valore picco sinistro
    {
        peakLeft.set(value);
    }
    
    if (channel == 1) //cambio valore picco destro
    {
        peakRight.set(value);
    }
    
}

//PRIVATE

void MeterPersonale::MeteringDsp::smoothValueRamp(float value, LinearSmoothedValue<float>& smoothedRmsVar)
{
    if(value < smoothedRmsVar.getCurrentValue() )
    {
        smoothedRmsVar.setTargetValue(value);
    }
    else
    {
        smoothedRmsVar.setCurrentAndTargetValue(value);
    }
}


void MeterPersonale::MeteringDsp::setStartingValues()
{
    peakLeft.set(-100.0f);
    peakRight.set(-100.0f);
    
    rmsLevelLeft.setCurrentAndTargetValue(-100.0f);
    rmsLevelRight.setCurrentAndTargetValue(-100.0f);
}
