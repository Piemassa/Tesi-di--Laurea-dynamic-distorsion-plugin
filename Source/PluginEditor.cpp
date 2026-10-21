
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Primo_distoAudioProcessorEditor::Primo_distoAudioProcessorEditor (Primo_distoAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), parameters(vts), meterIn(p.linkMeter(0)), meterOut(p.linkMeter(1))
{
    
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
    
  
    //drive by toggle button
    addAndMakeVisible(driveON);
    driveByAttachment = std::make_unique<ButtonAttachment>(parameters, "DRIVEBYPASS", driveON);
    driveON.setButtonText("Drive BY");
    driveON.setTooltip("drive circuit bypass");
    driveON.setClickingTogglesState(true);
    driveON.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::greenyellow);
    driveON.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::dimgrey);
    driveON.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::black);
    driveON.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::black);

    
    addAndMakeVisible(transientON);
    transientByAttachment = std::make_unique<ButtonAttachment>(parameters, "TRANSIENTBYPASS", transientON);
    transientON.setButtonText("Transient BY");
    transientON.setTooltip("transient circuit bypass");
    transientON.setClickingTogglesState(true);
    transientON.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::greenyellow);
    transientON.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::dimgrey);
    transientON.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::black);
    transientON.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::black);
    
    
    //drive amount
    addAndMakeVisible(driveAmountKnob);
    driveAmountKnob.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);

//    addAndMakeVisible(typeSelectorSlider);
//    typeSelectorSlider.setSliderStyle(Slider::LinearHorizontal);

    driveAmountAttachment = std::make_unique<SliderAttachment>(parameters, "DRIVEAMOUNT", driveAmountKnob);
//    typeSelectorAttachment = std::make_unique<SliderAttachment>(parameters, "TYPE", typeSelectorSlider);
    
    
    // transient shaper
    addAndMakeVisible(attackKnob);
    attackKnob.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    attackKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.setJustificationType(juce::Justification::centredRight);
    attackLabel.attachToComponent(&attackKnob, true);
    
    addAndMakeVisible(sustainKnob);
    sustainKnob.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    sustainKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    sustainLabel.setText("Sustain", juce::dontSendNotification);
    sustainLabel.setJustificationType(juce::Justification::centredRight);
    sustainLabel.attachToComponent(&sustainKnob, true);
    
    attackKnobAttachment = std::make_unique<SliderAttachment>(parameters, "ATTACK", attackKnob);
    sustainKnobAttachment = std::make_unique<SliderAttachment>(parameters, "SUSTAIN", sustainKnob);
    
    
    
    addAndMakeVisible(meterIn);
    addAndMakeVisible(meterOut);
    
    setSize (1000, 600);
};

Primo_distoAudioProcessorEditor::~Primo_distoAudioProcessorEditor()
{
};

//==============================================================================
void Primo_distoAudioProcessorEditor::paint (juce::Graphics& g)
{
   
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
};

void Primo_distoAudioProcessorEditor::resized()
{
    meterIn.Component::setBounds(20, 30, 60, 500);
    meterOut.Component::setBounds(920, 30, 60, 500);
    
    driveON.setBounds(430, 440, 60, 30);
    transientON.setBounds(830, 440, 60, 30);
    
    driveButton[0].setBounds(180 - 4, 60, 50, 30);
    driveButton[1].setBounds(240 - 4, 60, 50, 30);
    driveButton[2].setBounds(300 - 4, 60, 50, 30);
    driveButton[3].setBounds(360 - 4, 60, 50, 30);
    driveButton[4].setBounds(420 - 4, 60, 50, 30);
    
//    typeSelectorSlider.setBounds(50, 10, 200, 30);
    driveAmountKnob.setBounds(140, 170, 290, 290);
    
    attackKnob.setBounds(650, 130, 160, 160);
    sustainKnob.setBounds(650, 330, 160, 160);
    
    
}


void Primo_distoAudioProcessorEditor::updateModeButtons (float newValue)
{
    const int index = juce::roundToInt (newValue) - 1;

    if (juce::isPositiveAndBelow (index, 5))
    {
        driveButton[index].setToggleState (true, juce::dontSendNotification);
    }
}
