// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeWeaponDebugSettings.h"
#include "Misc/App.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeWeaponDebugSettings)

UPracticeWeaponDebugSettings::UPracticeWeaponDebugSettings()
{
}

FName UPracticeWeaponDebugSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}

