// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticePerfStatWidgetBase.h"

#include "Engine/GameInstance.h"
#include "Performance/PracticePerformanceStatSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticePerfStatWidgetBase)

//////////////////////////////////////////////////////////////////////
// UPracticePerfStatWidgetBase

UPracticePerfStatWidgetBase::UPracticePerfStatWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

double UPracticePerfStatWidgetBase::FetchStatValue()
{
	if (CachedStatSubsystem == nullptr)
	{
		if (UWorld* World = GetWorld())
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				CachedStatSubsystem = GameInstance->GetSubsystem<UPracticePerformanceStatSubsystem>();
			}
		}
	}

	if (CachedStatSubsystem)
	{
		return CachedStatSubsystem->GetCachedStat(StatToDisplay);
	}
	else
	{
		return 0.0;
	}
}

