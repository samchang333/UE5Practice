// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"

class UObject;

PRACTICEGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogPractice, Log, All);
PRACTICEGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogPracticeExperience, Log, All);
PRACTICEGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogPracticeAbilitySystem, Log, All);
PRACTICEGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogPracticeTeams, Log, All);

PRACTICEGAME_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
