// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "TTSStyle.h"

class FTTSCommands : public TCommands<FTTSCommands>
{
public:

	FTTSCommands()
		: TCommands<FTTSCommands>(TEXT("TTS"), NSLOCTEXT("Contexts", "TTS", "TTS Plugin"), NAME_None, FTTSStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};