
#pragma once

#include <JuceHeader.h>

namespace Parameters
{
    //==============================================================================
    //(ID) dei parametri
    static const String nameDriveLow  = "DRIVEAMOUNTLOW";
    static const String nameDriveMid  = "DRIVEAMOUNTMID";
    static const String nameDriveHigh = "DRIVEAMOUNTHIGH";

    static const String nameMode = "MODE";

    static const String nameAttackLow  = "ATTACKLOW";
    static const String nameAttackMid  = "ATTACKMID";
    static const String nameAttackHigh = "ATTACKHIGH";

    static const String nameSustainLow  = "SUSTAINLOW";
    static const String nameSustainMid  = "SUSTAINMID";
    static const String nameSustainHigh = "SUSTAINHIGH";

    static const String nameDriveBypassLow  = "DRIVEBYPASSLOW";
    static const String nameDriveBypassMid  = "DRIVEBYPASSMID";
    static const String nameDriveBypassHigh = "DRIVEBYPASSHIGH";

    static const String nameTransientBypassLow  = "TRANSIENTBYPASSLOW";
    static const String nameTransientBypassMid  = "TRANSIENTBYPASSMID";
    static const String nameTransientBypassHigh = "TRANSIENTBYPASSHIGH";

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

        params.push_back (std::make_unique<AudioParameterFloat> (ParameterID (nameDriveLow,  1), "DriveL (%)", NormalisableRange<float> (0.0f, 100.0f, 0.01f), defaultDrive));
        params.push_back (std::make_unique<AudioParameterFloat> (ParameterID (nameDriveMid,  1), "DriveM (%)", NormalisableRange<float> (0.0f, 100.0f, 0.01f), defaultDrive));
        params.push_back (std::make_unique<AudioParameterFloat> (ParameterID (nameDriveHigh, 1), "DriveH (%)", NormalisableRange<float> (0.0f, 100.0f, 0.01f), defaultDrive));

        params.push_back (std::make_unique<AudioParameterInt> (ParameterID (nameMode, 1), "Type", 1, 5, defaultMode));

        params.push_back (std::make_unique<AudioParameterFloat> (ParameterID (nameAttackLow,  1), "AttackLow",  NormalisableRange<float> (-1.0f, 1.0f, 0.001f), defaultAttack));
        params.push_back (std::make_unique<AudioParameterFloat> (ParameterID (nameAttackMid,  1), "AttackMid",  NormalisableRange<float> (-1.0f, 1.0f, 0.001f), defaultAttack));
        params.push_back (std::make_unique<AudioParameterFloat> (ParameterID (nameAttackHigh, 1), "AttackHigh", NormalisableRange<float> (-1.0f, 1.0f, 0.001f), defaultAttack));

        params.push_back (std::make_unique<AudioParameterFloat> (ParameterID (nameSustainLow,  1), "SustainLow",  NormalisableRange<float> (-1.0f, 1.0f, 0.001f), defaultSustain));
        params.push_back (std::make_unique<AudioParameterFloat> (ParameterID (nameSustainMid,  1), "SustainMid",  NormalisableRange<float> (-1.0f, 1.0f, 0.001f), defaultSustain));
        params.push_back (std::make_unique<AudioParameterFloat> (ParameterID (nameSustainHigh, 1), "SustainHigh", NormalisableRange<float> (-1.0f, 1.0f, 0.001f), defaultSustain));

        params.push_back (std::make_unique<AudioParameterBool> (ParameterID (nameDriveBypassLow,  1), "DriveBypass", defaultDriveBypass));
        params.push_back (std::make_unique<AudioParameterBool> (ParameterID (nameDriveBypassMid,  1), "DriveBypass", defaultDriveBypass));
        params.push_back (std::make_unique<AudioParameterBool> (ParameterID (nameDriveBypassHigh, 1), "DriveBypass", defaultDriveBypass));

        params.push_back (std::make_unique<AudioParameterBool> (ParameterID (nameTransientBypassLow,  1), "TransientBypass", defaultTransientBypass));
        params.push_back (std::make_unique<AudioParameterBool> (ParameterID (nameTransientBypassMid,  1), "TransientBypass", defaultTransientBypass));
        params.push_back (std::make_unique<AudioParameterBool> (ParameterID (nameTransientBypassHigh, 1), "TransientBypass", defaultTransientBypass));

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


