#include "BandComponent.h"


//==============================================================================
BandComponent::BandComponent (juce::AudioProcessorValueTreeState& apvts, int bandIndex)
    : band (bandIndex),
      driveKnob   (apvts, Parameters::nameDrive, "Drive"),
      attackKnob  (apvts, Parameters::nameAttack, "Attack"),
      sustainKnob (apvts, Parameters::nameSustain, "Sustain")
{
    driveByAttachment = std::make_unique<ButtonAttachment>(apvts, Parameters::nameDriveBypass, driveON );
    transientByAttachment = std::make_unique<ButtonAttachment>(apvts, Parameters::nameTransientBypass, transientON );
    
    addAndMakeVisible (driveKnob);
    addAndMakeVisible (attackKnob);
    addAndMakeVisible (sustainKnob);
    addAndMakeVisible (driveON);
    addAndMakeVisible (transientON);
    
    
    //================ bottoni bypass
    driveON.setButtonText("Drive BY");
    driveON.setTooltip("drive circuit bypass");
    driveON.setClickingTogglesState(true);
    driveON.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::greenyellow);
    driveON.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::dimgrey);
    driveON.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::black);
    driveON.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::black);
    
    transientON.setButtonText("Transient BY");
    transientON.setTooltip("transient circuit bypass");
    transientON.setClickingTogglesState(true);
    transientON.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::greenyellow);
    transientON.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::dimgrey);
    transientON.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::black);
    transientON.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::black);
    
    //================
    
    
    
}

BandComponent::~BandComponent()
{
}

//==============================================================================
void BandComponent::paint (juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat().reduced (2.0f);

    g.setColour (juce::Colours::white.withAlpha (0.05f));
    g.fillRoundedRectangle (r, 6.0f);

    g.setColour (juce::Colours::white.withAlpha (0.25f));
    g.drawRoundedRectangle (r, 6.0f, 1.0f);

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
    g.drawText ("band " JUCE_TO_STRING(bandIndex), getLocalBounds().removeFromTop (22), juce::Justification::centred);
}

void BandComponent::resized()
{
    auto area = getLocalBounds().reduced (6);

    // Titolo: altezza proporzionale, con limiti
    const int titleH = juce::jlimit (14, 22, static_cast<int> (area.getHeight() * 0.07f));
    area.removeFromTop (titleH);

    // Bottoni bypass: ~12% dell'altezza, ma mai meno di 16 né più di 30 px
    const int buttonH = juce::jlimit (16, 30, static_cast<int> (area.getHeight() * 0.12f));
    const int gap     = juce::jlimit (2, 6,  static_cast<int> (area.getHeight() * 0.02f));

    auto bottomButtonsArea = area.removeFromBottom (buttonH);
    area.removeFromBottom (gap);

    const int buttonWidth = static_cast<int> (bottomButtonsArea.getWidth() * 0.45f);
    driveON.setBounds     (bottomButtonsArea.removeFromLeft  (buttonWidth));
    transientON.setBounds (bottomButtonsArea.removeFromRight (buttonWidth));

    // Knob: occupano tutto lo spazio rimasto
    driveKnob.setBounds (area.removeFromLeft (static_cast<int> (area.getWidth() * 0.6f)));

    const int h = area.getHeight() / 2;
    attackKnob.setBounds  (area.removeFromTop (h));
    sustainKnob.setBounds (area);
}
