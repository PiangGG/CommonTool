// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "CommonToolStyle.h"

class FCommonToolCommands : public TCommands<FCommonToolCommands>
{
public:

	FCommonToolCommands()
		: TCommands<FCommonToolCommands>(TEXT("CommonTool"), NSLOCTEXT("Contexts", "CommonTool", "CommonTool Plugin"), NAME_None, FCommonToolStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > GenerateDevice;
};
