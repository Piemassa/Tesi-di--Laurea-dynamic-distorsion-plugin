
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Primo_distoAudioProcessorEditor::Primo_distoAudioProcessorEditor (Primo_distoAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), parameters(vts), meterIn(p.linkMeter(0)), meterOut(p.linkMeter(1))
{
    
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
    
    //GAIN KNOB
    inputGainKnob  = std::make_unique<LabeledKnob> (parameters, Parameters::nameGainIn,  "GainIn");
    outputGainKnob = std::make_unique<LabeledKnob> (parameters, Parameters::nameGainOut, "GainOut");
    
    addAndMakeVisible(*inputGainKnob);
    addAndMakeVisible(*outputGainKnob);

    //BANDA
    band1 = std::make_unique<BandComponent>(parameters, 1);
    addAndMakeVisible(*band1);
    
    addAndMakeVisible(meterIn);
    addAndMakeVisible(meterOut);
    
    setResizable (true, true);
    setResizeLimits (500, 400, 1600, 900);
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
    auto area = getLocalBounds().reduced (20);

    const int knobH = juce::jlimit (80, 120, area.getHeight() / 4);
    const int colW  = juce::jlimit (60, 90, getWidth() / 14);

    auto leftCol  = area.removeFromLeft  (colW);
    auto rightCol = area.removeFromRight (colW);

    // KNOB VOLUME (sotto i meter)
    if (inputGainKnob != nullptr)
        inputGainKnob->setBounds (leftCol.removeFromBottom (knobH));

    if (outputGainKnob != nullptr)
        outputGainKnob->setBounds (rightCol.removeFromBottom (knobH));

    // METER
    meterIn.Component::setBounds  (leftCol.withTrimmedBottom (6));
    meterOut.Component::setBounds (rightCol.withTrimmedBottom (6));

    area.reduce (10, 0);

    auto bottom = area.removeFromBottom (40);

    // DRIVE MODE (a sinistra)
    const int buttonWidth = 60;
    auto driveArea = bottom.removeFromLeft (buttonWidth * juce::numElementsInArray (driveButton));

    for (auto& button : driveButton)
        button.setBounds (driveArea.removeFromLeft (buttonWidth).reduced (2, 5));

    // MODULE ORDER (nello spazio rimasto)
    const int moduleWidth = juce::jmin (150, bottom.getWidth());
    moduleOrder.setBounds (bottom.withSizeKeepingCentre (moduleWidth, 30));

    // BANDA
    if (band1 != nullptr)
        band1->setBounds (area);
}

void Primo_distoAudioProcessorEditor::updateModeButtons (float newValue)
{
    const int index = juce::roundToInt (newValue) - 1;

    if (juce::isPositiveAndBelow (index, 5))
    {
        driveButton[index].setToggleState (true, juce::dontSendNotification);
    }
}
