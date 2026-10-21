
#include <JuceHeader.h>
#include "PeakMeter.h"




PeakMeter::PeakMeter()
{
    alpha = exp(-1.0/(FPS * RT));
}

PeakMeter::~PeakMeter()
{
}

void PeakMeter::paint (juce::Graphics& g)
{

//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
//    g.fillAll(Colours::yellow);
//    
    const auto bounds = getLocalBounds().toFloat();
    ColourGradient gradient(
        Colours::green.withAlpha(0.5f).withBrightness(0.7),
        bounds.getBottomLeft(),
        Colours::red.withAlpha(0.5f).withBrightness(0.7),
        bounds.getTopLeft(),
        false
    );
    gradient.addColour(0.7f, Colours::greenyellow.withAlpha(0.5f).withBrightness(0.7));
    gradient.addColour(0.85f, Colours::orange.withAlpha(0.5f).withBrightness(0.7));
    
    
    
    float W = getWidth();
    float H = getHeight();
    
//    if (probedSignal !=  nullptr)
//    {
//        float peak = probedSignal->get() * alpha; // in questo punto mi prendo il valore in db del picco da MeteringPocess e lo rettifico per alpha
//        
//        auto barHeight = jmap(peak, DB_FLOOR, DB_LIMIT, 0.0f, H );
//        barHeight = jlimit(0.0f, H , barHeight);
//        
//        DBG(barHeight);
////        auto color = peak >= 0.0f ? Colours::red : Colours::green;
////        g.setColour(color);
//        g.setGradientFill(gradient);
//        Rectangle<float> boundsPerMe(1.0f, 1.0f, W, H);
//        g.fillRoundedRectangle(boundsPerMe.removeFromBottom(barHeight), 4.0f);
//
//    }
    if (probedSignal !=  nullptr)
    {
        float peak = probedSignal->get() * alpha; // in questo punto mi prendo il valore in db del picco da MeteringPocess e lo rettifico per alpha
        
//        auto barHeight = jmap(peak, DB_FLOOR, DB_LIMIT, 0.0f, H );
//        barHeight = jlimit(0.0f, H , barHeight);
        
        
        peak = jlimit(DB_FLOOR, DB_LIMIT, peak);
        auto Temp= (peak - DB_FLOOR) / (DB_LIMIT - DB_FLOOR);
//        Temp = jlimit(DB_FLOOR, DB_LIMIT , Temp);
        float Temp2 = pow(Temp, 2.5f);
        float barHeight = H * Temp2;
        
        barHeight = jlimit(0.0f, H , barHeight);
        DBG("Peak");
        DBG(peak);
        DBG("Temp");
        DBG(Temp);
        DBG("Temp2");
        DBG(Temp2);
        DBG(barHeight);

        g.setGradientFill(gradient);
        Rectangle<float> boundsPerMe(1.0f, 1.0f, W, H);
        g.fillRoundedRectangle(boundsPerMe.removeFromBottom(barHeight), 4.0f);

    }
   
    
}

void PeakMeter::resized()
{

}



void PeakMeter::connectTo(Atomic<float>& signalSnapshot)
{
    probedSignal = &signalSnapshot;
}
