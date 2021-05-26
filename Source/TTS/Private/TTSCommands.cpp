// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TTSCommands.h"

#define LOCTEXT_NAMESPACE "FTTSModule"

void FTTSCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "TTS", "Bring up TTS window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
