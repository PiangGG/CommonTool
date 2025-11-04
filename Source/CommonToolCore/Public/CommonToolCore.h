// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "Logging/LogMacros.h"
DECLARE_LOG_CATEGORY_EXTERN(CommonToolCoreLog, Log, All);

class FCommonToolCoreModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
