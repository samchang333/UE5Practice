// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticePerformanceSettings.h"

#include "Engine/PlatformSettingsManager.h"
#include "Misc/EnumRange.h"
#include "Performance/PracticePerformanceStatTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticePerformanceSettings)

//////////////////////////////////////////////////////////////////////

UPracticePlatformSpecificRenderingSettings::UPracticePlatformSpecificRenderingSettings()
{
	MobileFrameRateLimits.Append({ 20, 30, 45, 60, 90, 120 });
}

const UPracticePlatformSpecificRenderingSettings* UPracticePlatformSpecificRenderingSettings::Get()
{
	UPracticePlatformSpecificRenderingSettings* Result = UPlatformSettingsManager::Get().GetSettingsForPlatform<ThisClass>();
	check(Result);
	return Result;
}

//////////////////////////////////////////////////////////////////////

UPracticePerformanceSettings::UPracticePerformanceSettings()
{
	PerPlatformSettings.Initialize(UPracticePlatformSpecificRenderingSettings::StaticClass());

	CategoryName = TEXT("Game");

	DesktopFrameRateLimits.Append({ 30, 60, 120, 144, 160, 165, 180, 200, 240, 360 });

	// Default to all stats are allowed
	FPracticePerformanceStatGroup& StatGroup = UserFacingPerformanceStats.AddDefaulted_GetRef();
	for (EPracticeDisplayablePerformanceStat PerfStat : TEnumRange<EPracticeDisplayablePerformanceStat>())
	{
		StatGroup.AllowedStats.Add(PerfStat);
	}
}

