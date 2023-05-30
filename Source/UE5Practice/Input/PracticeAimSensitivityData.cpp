// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeAimSensitivityData.h"

#include "Settings/PracticeSettingsShared.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeAimSensitivityData)

UPracticeAimSensitivityData::UPracticeAimSensitivityData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SensitivityMap =
	{
		{ EPracticeGamepadSensitivity::Slow,			0.5f },
		{ EPracticeGamepadSensitivity::SlowPlus,		0.75f },
		{ EPracticeGamepadSensitivity::SlowPlusPlus,	0.9f },
		{ EPracticeGamepadSensitivity::Normal,		1.0f },
		{ EPracticeGamepadSensitivity::NormalPlus,	1.1f },
		{ EPracticeGamepadSensitivity::NormalPlusPlus,1.25f },
		{ EPracticeGamepadSensitivity::Fast,			1.5f },
		{ EPracticeGamepadSensitivity::FastPlus,		1.75f },
		{ EPracticeGamepadSensitivity::FastPlusPlus,	2.0f },
		{ EPracticeGamepadSensitivity::Insane,		2.5f },
	};
}

const float UPracticeAimSensitivityData::SensitivtyEnumToFloat(const EPracticeGamepadSensitivity InSensitivity) const
{
	if (const float* Sens = SensitivityMap.Find(InSensitivity))
	{
		return *Sens;
	}

	return 1.0f;
}

