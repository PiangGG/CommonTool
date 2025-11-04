// Copyright Epic Games, Inc. All Rights Reserved.

#include "CommonToolCommands.h"

#define LOCTEXT_NAMESPACE "FCommonTestModule"

void FCommonToolCommands::RegisterCommands()
{
	UI_COMMAND(GenerateDevice, "CommonTool", "打开设备生成工具", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
