// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"

class UObject;

UE5PRACTICE_API DECLARE_LOG_CATEGORY_EXTERN(LogPractice, Log, All);
UE5PRACTICE_API DECLARE_LOG_CATEGORY_EXTERN(LogPracticeExperience, Log, All);
UE5PRACTICE_API DECLARE_LOG_CATEGORY_EXTERN(LogPracticeAbilitySystem, Log, All);
UE5PRACTICE_API DECLARE_LOG_CATEGORY_EXTERN(LogPracticeTeams, Log, All);

UE5PRACTICE_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
