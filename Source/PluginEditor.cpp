
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Primo_distoAudioProcessorEditor::Primo_distoAudioProcessorEditor (Primo_distoAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), parameters(vts)
{
    
    //drive amount
    driveAmountSlider.setSliderStyle(Slider::LinearVertical);
    driveAmountSlider.setBounds(8, 72, 104, 100);
    addAndMakeVisible(driveAmountSlider);
    
    typeSelectorSlider.setSliderStyle(Slider::LinearHorizontal);
    typeSelectorSlider.setBounds(10, 10, 200, 30);
    addAndMakeVisible(typeSelectorSlider);
    
    driveAmountAttachment = std::make_unique<SliderAttachment>(parameters, "DRIVEAMOUNT", driveAmountSlider);
    typeSelectorAttachment = std::make_unique<SliderAttachment>(parameters, "TYPE", typeSelectorSlider);
    
    
    // transient shaper
    attackKnob.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    attackKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    attackKnob.setBounds(220, 10, 170, 60);
    addAndMakeVisible(attackKnob);
    
    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.setJustificationType(juce::Justification::centredRight);
    attackLabel.attachToComponent(&attackKnob, true);
    
    
    sustainKnob.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    sustainKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    sustainKnob.setBounds(220, 50, 170, 60);
    addAndMakeVisible(sustainKnob);
    
    sustainLabel.setText("Sustain", juce::dontSendNotification);
    sustainLabel.setJustificationType(juce::Justification::centredRight);
    sustainLabel.attachToComponent(&sustainKnob, true);
    
    
    attackKnobAttachment = std::make_unique<SliderAttachment>(parameters, "ATTACK", attackKnob);
    sustainKnobAttachment = std::make_unique<SliderAttachment>(parameters, "SUSTAIN", sustainKnob);
    
    
    setSize (500, 400);
}

Primo_distoAudioProcessorEditor::~Primo_distoAudioProcessorEditor()
{
}

//==============================================================================
void Primo_distoAudioProcessorEditor::paint (juce::Graphics& g)
{
   
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World! ciaoneadcadc", getLocalBounds(), juce::Justification::centred, 1);
}

void Primo_distoAudioProcessorEditor::resized()
{
 
}
