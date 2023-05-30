// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeGameData.h"
#include "PracticeAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeGameData)

UPracticeGameData::UPracticeGameData()
{
}

const UPracticeGameData& UPracticeGameData::UPracticeGameData::Get()
{
	return UPracticeAssetManager::Get().GetGameData();
}
