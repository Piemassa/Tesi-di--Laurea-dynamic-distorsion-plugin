#include "Distorsori.h"

Distorsion_P::Distorsion_P()
{
}

Distorsion_P::~Distorsion_P()
{
}
void Distorsion_P::prepareToPlay()
{
    
}

void Distorsion_P::processBlock(float &sampleToProcess)
{
    
    switch (readDistType()) {
        case 0:
            break;
        case 1:
            Tanh(sampleToProcess, read_DriveAmount());
            break;
        case 2:
            softSign(sampleToProcess, read_DriveAmount());
            break;
        case 3:
            TapeLike2(sampleToProcess, read_DriveAmount());
            break;
        case 4:
            disto4(sampleToProcess, read_DriveAmount());
            break;
        case 5:
            turboDistorto(sampleToProcess, read_DriveAmount());
            break;
        case 6:
            wavefolding(sampleToProcess, read_DriveAmount());
        default:
            break;
    }
    
    
}

int Distorsion_P::readDistType()
{
    return dist_type;
}

void Distorsion_P::writeDistType(int newValue)
{
    jassert(newValue >= 0);
    dist_type = newValue;
}

float Distorsion_P::read_DriveAmount()
{
    return driveAmount.value;
}

void Distorsion_P::write_DriveAmount(float newValue)
{
    driveAmount.set(newValue);
}



void Distorsion_P::Tanh(float &sample, float driveAmount ) //type 1
{
    driveAmount += 1;
    auto temp = sample * driveAmount;
    sample = tanh(temp);
    sample = sample / std::sqrt(driveAmount);
    
}

void Distorsion_P::softSign(float &sample, float driveAmount) //type 2
{
    driveAmount += 1;
    auto temp = sample * driveAmount;
    sample = temp / (1 + abs(temp));
    sample = sample / std::sqrt(driveAmount);

}

void Distorsion_P::TapeLike2(float &sample, float driveAmount) //type 3
{
    driveAmount += 1;
    auto k = 3.782;
    auto temp = sample * driveAmount;
    sample =  tanh( k * temp) / tanh(k);
    sample = sample / std::sqrt(driveAmount);
    
}

void Distorsion_P::disto4(float &sample, float driveAmount) //type 4
{
    driveAmount += 1;
    auto k = driveAmount;
    sample = sample + (  (k/(juce::MathConstants<float>::twoPi)) * sin(juce::MathConstants<float>::twoPi*sample) );
    sample = sample / (1.0f + (k * 0.3f));
}

void Distorsion_P::turboDistorto(float &sample, float driveAmount) //turbodrive di pirkle
{
    driveAmount += 1;
    auto k = driveAmount;
    auto m = k/3.0f;
    
    //VWS1
    
    sample = (tanh(m * sample) / tanh(m));
    
    //VWS2
    
    sample = (tanh(k * sample) / tanh(k));
    
    sample = sample / std::sqrt(std::sqrt(k));
    
}

void Distorsion_P::wavefolding(float &sample, float driveAmount)
{
    driveAmount += 1;
    auto temp = sample * driveAmount;
    sample = processSampleHermite(temp);
    
    sample = sample / std::sqrt(driveAmount);
    
}


float Distorsion_P::evalHermite(float t, float p0, float p1, float m0, float m1)
{
    float t2 = t * t;
    float t3 = t2 * t;

    float h00 =  2.0f * t3 - 3.0f * t2 + 1.0f;
    float h10 =         t3 - 2.0f * t2 + t;
    float h01 = -2.0f * t3 + 3.0f * t2;
    float h11 =         t3 -        t2;

    return h00 * p0 + h10 * m0 + h01 * p1 + h11 * m1;
}


float Distorsion_P::processSampleHermite(float x)
{
    // 1. Gestione della simmetria dispari (f(-x) = -f(x))
    float sign = (x < 0.0f) ? -1.0f : 1.0f;
    float xAbs = std::abs(x);

    // Hard clipping di sicurezza oltre |x| = 1.0
    if (xAbs >= 1.0f)
        return sign * 0.95f;

    // 2. Definizione dei punti chiave dal grafico GeoGebra
    // Tratto 1: da x=0.0 a x=0.4 (Zona Crossover flessuosa)
    if (xAbs < 0.4f)
    {
        float t = xAbs / 0.4f; // Normalizza xAbs nell'intervallo [0, 1]
        
        // p0=0.0, p1=0.25
        // m0=0.1 (pendenza quasi piatta a zero), m1=1.5 (pendenza ripida all'uscita)
        return sign * evalHermite(t, 0.0f, 0.25f, 0.1f, 1.5f);
    }
    // Tratto 2: da x=0.4 a x=1.0 (Zona Rampa e Soft Saturation)
    else
    {
        float t = (xAbs - 0.4f) / 0.6f; // Normalizza xAbs nell'intervallo [0, 1]
        
        // p0=0.25, p1=0.95
        // m0=1.5 (continuità della pendenza dal tratto 1), m1=0.05 (saturazione quasi piatta in alto)
        return sign * evalHermite(t, 0.25f, 0.95f, 1.5f, 0.05f);
    }
}
