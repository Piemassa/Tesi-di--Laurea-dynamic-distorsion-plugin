#pragma once

#include <JuceHeader.h>

namespace Parameters
{
    //==============================================================================
    //(ID) dei parametri
    static const String nameDrive  = "DRIVEAMOUNT";

    static const String nameMode = "MODE";

    static const String nameAttack  = "ATTACK";

    static const String nameSustain  = "SUSTAIN";

    static const String nameDriveBypass  = "DRIVEBYPASS";

    static const String nameTransientBypass  = "TRANSIENTBYPASS";

    static const String nameOrder = "ORDER";

    //==============================================================================
    // Valori di default
    static const float defaultDrive           = 0.0f;
    static const int   defaultMode            = 1;
    static const float defaultAttack          = 0.0f;
    static const float defaultSustain         = 0.0f;
    static const bool  defaultDriveBypass     = true;
    static const bool  defaultTransientBypass = true;
    static const int   defaultOrder           = 0;

    //==============================================================================
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        std::vector<std::unique_ptr<RangedAudioParameter>> params;

        params.push_back (std::make_unique<AudioParameterFloat> (ParameterID (nameDrive,  1), "DriveL (%)", NormalisableRange<float> (0.0f, 100.0f, 0.01f), defaultDrive));

        params.push_back (std::make_unique<AudioParameterInt> (ParameterID (nameMode, 1), "Type", 1, 5, defaultMode));

        params.push_back (std::make_unique<AudioParameterFloat> (ParameterID (nameAttack,  1), "Attack",  NormalisableRange<float> (-1.0f, 1.0f, 0.001f), defaultAttack));

        params.push_back (std::make_unique<AudioParameterFloat> (ParameterID (nameSustain,  1), "SustainLow",  NormalisableRange<float> (-1.0f, 1.0f, 0.001f), defaultSustain));


        params.push_back (std::make_unique<AudioParameterBool> (ParameterID (nameDriveBypass,  1), "DriveBypass", defaultDriveBypass));

        params.push_back (std::make_unique<AudioParameterBool> (ParameterID (nameTransientBypass,  1), "TransientBypass", defaultTransientBypass));

        params.push_back (std::make_unique<AudioParameterInt> (ParameterID (nameOrder, 1), "Order", 0, 1, defaultOrder));

        return { params.begin(), params.end() };
    }

    //==============================================================================

    static void addListenerToAllParameters (AudioProcessorValueTreeState& valueTreeState,
                                            AudioProcessorValueTreeState::Listener* listener)
    {
        for (auto* p : valueTreeState.processor.getParameters())
            if (auto* withID = dynamic_cast<AudioProcessorParameterWithID*> (p))
                valueTreeState.addParameterListener (withID->paramID, listener);
    }
}


