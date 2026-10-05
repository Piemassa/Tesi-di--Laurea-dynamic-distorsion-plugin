#include "TransientShaper.h"

Transient_Shaper::Transient_Shaper()
{
    
};

Transient_Shaper::~Transient_Shaper()
{
    
};

void Transient_Shaper::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    
    // chiamo prepare to play
    fastEnvelope.prepareToPlay(sampleRate, samplesPerBlock);
    slowEnvelope.prepareToPlay(sampleRate, samplesPerBlock);
    
    //setto attack and release fast
    fastEnvelope.setAttackTime(1.0f);
    fastEnvelope.setReleaseTime(30.0f);
    
    //setto attack and release slow
    slowEnvelope.setAttackTime(20.0f);
    slowEnvelope.setReleaseTime(300.0f);
    
};


void Transient_Shaper::processBlock(float workingSample)
{
    //prende sample in entrata
    
    
    //calcola inviluppo
    fastEnvelope.processBlock(workingSample);
    slowEnvelope.processBlock(workingSample);
    
    //calcolo transiente differenziale
    // Separa Attacco e Coda
    attackDifferential  = std::max(0.0f, fastEnvelope.getCurrentEnvelope() - slowEnvelope.getCurrentEnvelope());
    sustainDifferential = std::max(0.0f, slowEnvelope.getCurrentEnvelope() - fastEnvelope.getCurrentEnvelope());
    
    //test per espandere i valori negativi
    sustainDifferential = std::sqrt(sustainDifferential);
    
    
    // Scala asimmetrica: 4.0x in boost 1.0x in cut
    float attackScale  = (attackKnob > 0.0f)  ? 4.0f : 1.0f;
    float sustainScale = (sustainKnob > 0.0f) ? 4.0f : 1.0f;
    
    float attackGainMod = attackDifferential * (attackKnob * attackScale);
    float sustainGainMod = sustainDifferential * (sustainKnob * sustainScale);
    

    //modifica il gain basandosi sui dati dell'inviluppo
    float newGainModulation = 1.0f + attackGainMod + sustainGainMod;
    
    newGainModulation = std::max(0.0f, newGainModulation);
    
    //aggiorno il valore di gain modulation
    
    setGainModulation(newGainModulation);
};

void Transient_Shaper::setGainModulation(float newValue)
{
    gainModulation = newValue;
};

float Transient_Shaper::getGainValue()
{
    return gainModulation;
};

void Transient_Shaper::writeAttackKnob(float newValue)
{
    attackKnob = newValue;
};

float Transient_Shaper::readAttackKnob()
{
    return attackKnob;
};

void Transient_Shaper::writeSustainKnob(float newValue)
{
    sustainKnob = newValue;
};

float Transient_Shaper::readSustainKnob()
{
    return sustainKnob;
};
