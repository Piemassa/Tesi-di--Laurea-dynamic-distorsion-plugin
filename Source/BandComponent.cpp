#include "BandComponent.h"


namespace
{
    // Dato l'indice della banda (0 = Low, 1 = Mid, 2 = High)
    const juce::String& driveID (int band)
    {
        switch (band)
        {
            case 0:
                return Parameters::nameDriveLow;
            case 1:
                return Parameters::nameDriveMid;
            default:
            {
                jassert (band == 2);
                return Parameters::nameDriveHigh;
            }
        }
    }

    const juce::String& attackID (int band)
    {
        switch (band)
        {
            case 0:
                return Parameters::nameAttackLow;
            case 1:
                return Parameters::nameAttackMid;
            default:
            {
                jassert (band == 2);
                return Parameters::nameAttackHigh;
            }
        }
    }

    const juce::String& sustainID (int band)
    {
        switch (band)
        {
            case 0:
                return Parameters::nameSustainLow;
            case 1:
                return Parameters::nameSustainMid;
            default:
            {
                jassert (band == 2);
                return Parameters::nameSustainHigh;
            }
        }
    }


    const juce::String& driveBypassID (int band)
    {
        switch (band)
        {
            case 0:
                return Parameters::nameDriveBypassLow;
            case 1:
                return Parameters::nameDriveBypassMid;
            default:
            {
                jassert (band == 2);
                return Parameters::nameDriveBypassHigh;
            }
        }
    }

    const juce::String& transientBypassID (int band)
    {
        switch (band)
        {
            case 0:
                return Parameters::nameTransientBypassLow;
            case 1:
                return Parameters::nameTransientBypassMid;
            default:
            {
                jassert (band == 2);
                return Parameters::nameTransientBypassHigh;
            }
        }
    }

    juce::String bandTitle (int band)
    {
        switch (band)
        {
            case 0:  return "LOW";
            case 1:  return "MID";
            default: return "HIGH";
        }
    }
};





//==============================================================================
BandComponent::BandComponent (juce::AudioProcessorValueTreeState& apvts, int bandIndex)
    : band (bandIndex),
      driveKnob   (apvts, driveID   (bandIndex), "Drive"),
      attackKnob  (apvts, attackID  (bandIndex), "Attack"),
      sustainKnob (apvts, sustainID (bandIndex), "Sustain")
{
    driveByAttachment = std::make_unique<ButtonAttachment>(apvts, driveBypassID(bandIndex), driveON );
    transientByAttachment = std::make_unique<ButtonAttachment>(apvts, transientBypassID(bandIndex), transientON );
    
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
    g.drawText (bandTitle (band), getLocalBounds().removeFromTop (22), juce::Justification::centred);
}

void BandComponent::resized()
{
    auto area = getLocalBounds().reduced (8);
    area.removeFromTop (20);                       //titolo


    auto bottomButtonsArea = area.removeFromBottom (30);
    area.removeFromBottom (6);                     // piccolo spazio tra knob e bottoni

    const int buttonWidth = static_cast<int> (bottomButtonsArea.getWidth() * 0.42f);

    driveON.setBounds     (bottomButtonsArea.removeFromLeft  (buttonWidth));
    transientON.setBounds (bottomButtonsArea.removeFromRight (buttonWidth));

    // Drive grande a sinistra (60%), Attack e Sustain impilati a destra
    driveKnob.setBounds (area.removeFromLeft (static_cast<int> (area.getWidth() * 0.6f)));

    const int h = area.getHeight() / 2;
    attackKnob.setBounds  (area.removeFromTop (h));
    sustainKnob.setBounds (area);
}
