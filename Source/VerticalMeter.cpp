
#include <JuceHeader.h>
#include "VerticalMeter.h"


#define FPS 30

VerticalMeter::VerticalMeter(MeterPersonale::MeteringDsp& linkRef)
{
    
    leftPeak.connectTo(linkRef.getPeakVariable(0)); //sinistro
    addAndMakeVisible(leftPeak);
    
    rightPeak.connectTo(linkRef.getPeakVariable(1)); //destro
    addAndMakeVisible(rightPeak);
    
    startTimerHz(FPS);
}

VerticalMeter::~VerticalMeter()
{
    stopTimer();
}

void VerticalMeter::paint (juce::Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
//    g.fillAll(Colours::red);
    //rettangolo di fondo
    g.setColour(Colours::black.withBrightness(0.25f).withAlpha(0.7f));

    g.fillRect(20, 0, getWidth(), getHeight() );
    
    g.setColour(Colours::white.withBrightness(0.9f).withAlpha(0.9f));
    
    for (float dB : DB_VALUES_MARK)
    {
        
//        float y = jmap(dB, DB_FLOOR,DB_LIMIT, static_cast<float>(getHeight()), 0.0f); //mi ritorna un valore del mio db mappato per altezzza
        
        auto Temp = (dB - DB_FLOOR) / (DB_LIMIT - DB_FLOOR);
        auto Temp2 = pow(Temp, 2.5f);
        float y = getHeight() * Temp2;
        
        g.drawLine(19.5f,  getHeight() - y, getWidth() , getHeight() - y, 1.0f);
        
        g.setFont(FontOptions("Helvetica",11.0f, Font::bold));
        g.drawText(String(dB), 0,  getHeight() - y - 8, 19, 16, Justification::right);
    }

    
}

void VerticalMeter::resized()
{

    auto W = (getWidth() - 20)/2;
    auto H = getHeight();
    
    leftPeak.setBounds(0.0f + 20.0f,     0.0f,   W-1.0f,H );
    rightPeak.setBounds(0.0f+ W + 20.0f, 0.0f,   W-1.0f,H );
    
}



void VerticalMeter::timerCallback()
{
    repaint();
}
