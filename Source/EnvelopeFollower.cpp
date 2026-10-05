#include "EnvelopeFollower.h"


Envelope_Follower::Envelope_Follower()
{
    
};

Envelope_Follower::~Envelope_Follower()
{
    
};


void Envelope_Follower::prepareToPlay(double newSampleRate, int samplesPerBlock)
{
    sampleRate = newSampleRate;
    updateAttackCoeff();
    updateReleaseCoeff();
    reset();
    
}


void Envelope_Follower::processBlock(float SampleToProcess)
{

    // 2. Selezione dinamica del coefficiente (Attacco vs Rilascio)
    float coef = (SampleToProcess > currentEnvelope) ? attackCoef : releaseCoef;

    // 3. Filtro IIR del primo ordine (Passa-Basso)
    currentEnvelope = coef * currentEnvelope + (1.0f - coef) * SampleToProcess;
    
}



void Envelope_Follower::reset()
{
    currentEnvelope = 0.0f;
}


void Envelope_Follower::setAttackTime(float attackMs)
{
    attackTimeMs = std::max(0.001f, attackMs); // Evita divisioni per zero
    updateAttackCoeff();
}


void Envelope_Follower::setReleaseTime(float releaseMs)
{
    releaseTimeMs = std::max(0.001f, releaseMs);
    updateReleaseCoeff();
}


void Envelope_Follower::updateAttackCoeff()
{
    if (sampleRate > 0.0)
    attackCoef = std::exp(-1.0f / (static_cast<float>(sampleRate) * (attackTimeMs * 0.001f)));
}


void Envelope_Follower::updateReleaseCoeff()
{
    if (sampleRate > 0.0)
    releaseCoef = std::exp(-1.0f / (static_cast<float>(sampleRate) * (releaseTimeMs * 0.001f)));

}

float Envelope_Follower::getCurrentEnvelope()
{
    return currentEnvelope;
}
