
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Primo_distoAudioProcessorEditor::Primo_distoAudioProcessorEditor (Primo_distoAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), parameters(vts), meterIn(p.linkMeter(0)), meterOut(p.linkMeter(1))
{
    //module order button
    addAndMakeVisible(moduleOrder);
    moduleOrder.setTooltip("module order");
    moduleOrder.addItem("Transient --> Distorsion", 1);
    moduleOrder.addItem("Distorsion --> Transient", 2);
    moduleOrderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(parameters,"ORDER", moduleOrder);
  
    
    
    const int driveModeID = 1001;
    //recuperiamo il parametro mode tramite parameters
    auto* modeParam = parameters.getParameter("MODE");
    
    for(int i = 0; i < 5; ++i)
    {
        driveButton[i].setButtonText("MODE " + juce::String( i + 1) );
        driveButton[i].setClickingTogglesState(true);
        driveButton[i].setRadioGroupId(driveModeID);
        driveButton[i].setTooltip("Distorsion circuit " + juce::String(i + 1));
        addAndMakeVisible(driveButton[i]);
        
    }
    
    if(modeParam !=nullptr)
    {
        modeAttachment = std::make_unique<juce::ParameterAttachment>(*modeParam, [this] (float value) { updateModeButtons (value); },nullptr);
        
        for (int i = 0; i < 5; ++i)
            {
                driveButton[i].onClick = [this, i]()
                {
                    if (driveButton[i].getToggleState())
                    {
                        modeAttachment->setValueAsPartOfGesture (static_cast<float> (i + 1));
                    }
                };
            }
        modeAttachment->sendInitialUpdate();
        
    }
    
    // METER
    addAndMakeVisible(meterIn);
    addAndMakeVisible(meterOut);
    
    
    //BAND
    bandLow = std::make_unique<BandComponent>(parameters, 0);
    bandMid = std::make_unique<BandComponent>(parameters, 1);
    bandHigh = std::make_unique<BandComponent>(parameters, 2);
    
    addAndMakeVisible(*bandLow);
    addAndMakeVisible(*bandMid);
    addAndMakeVisible(*bandHigh);
    
    setResizable (true, true);
    setResizeLimits (800, 500, 1600, 900);
    setSize (1000, 600);
};

Primo_distoAudioProcessorEditor::~Primo_distoAudioProcessorEditor()
{
};

//==============================================================================
void Primo_distoAudioProcessorEditor::paint (juce::Graphics& g)
{
   
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

};

void Primo_distoAudioProcessorEditor::resized()
{
    
    auto area = getLocalBounds().reduced(20);
    
    //METER
    meterIn.Component::setBounds  (area.removeFromLeft (60));
    meterOut.Component::setBounds (area.removeFromRight (60));
    area.reduce (10, 0);
    
    
    auto bottom = area.removeFromBottom (40);

    //MODULE ORDER
    moduleOrder.setBounds (bottom.withSizeKeepingCentre (150, 30));

    //DRIVE MODE
    for (auto& button : driveButton)
    button.setBounds (bottom.removeFromLeft (60).reduced (2, 5));
    
    // BANDE
    const int bandWidth = area.getWidth() / 3;
    bandLow->setBounds  (area.removeFromLeft (bandWidth));
    bandMid->setBounds  (area.removeFromLeft (bandWidth));
    bandHigh->setBounds (area);
    
//    driveButton[0].setBounds(180 - 90, 549, 50, 30);
//    driveButton[1].setBounds(240 - 90, 549, 50, 30);
//    driveButton[2].setBounds(300 - 90, 549, 50, 30);
//    driveButton[3].setBounds(360 - 90, 549, 50, 30);
//    driveButton[4].setBounds(420 - 90, 549, 50, 30);
//    
//    //module order button
//    moduleOrder.setBounds(425, 549, 150, 30);
    
    
}


void Primo_distoAudioProcessorEditor::updateModeButtons (float newValue)
{
    const int index = juce::roundToInt (newValue) - 1;

    if (juce::isPositiveAndBelow (index, 5))
    {
        driveButton[index].setToggleState (true, juce::dontSendNotification);
    }
}
