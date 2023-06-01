// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetTypeActions_PracticeContextEffectsLibrary.h"

#include "Feedback/ContextEffects/PracticeContextEffectsLibrary.h"

class UClass;

#define LOCTEXT_NAMESPACE "AssetTypeActions"

UClass* FAssetTypeActions_PracticeContextEffectsLibrary::GetSupportedClass() const
{
	return UPracticeContextEffectsLibrary::StaticClass();
}

#undef LOCTEXT_NAMESPACE
