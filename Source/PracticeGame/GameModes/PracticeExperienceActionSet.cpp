// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeExperienceActionSet.h"
#include "GameFeatureAction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeExperienceActionSet)

#define LOCTEXT_NAMESPACE "PracticeSystem"

UPracticeExperienceActionSet::UPracticeExperienceActionSet()
{
}

#if WITH_EDITOR
EDataValidationResult UPracticeExperienceActionSet::IsDataValid(TArray<FText>& ValidationErrors)
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(ValidationErrors), EDataValidationResult::Valid);

	int32 EntryIndex = 0;
	for (UGameFeatureAction* Action : Actions)
	{
		if (Action)
		{
			EDataValidationResult ChildResult = Action->IsDataValid(ValidationErrors);
			Result = CombineDataValidationResults(Result, ChildResult);
		}
		else
		{
			Result = EDataValidationResult::Invalid;
			ValidationErrors.Add(FText::Format(LOCTEXT("ActionEntryIsNull", "Null entry at index {0} in Actions"), FText::AsNumber(EntryIndex)));
		}

		++EntryIndex;
	}

	return Result;
}
#endif

#if WITH_EDITORONLY_DATA
void UPracticeExperienceActionSet::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();

	for (UGameFeatureAction* Action : Actions)
	{
		if (Action)
		{
			Action->AddAdditionalAssetBundleData(AssetBundleData);
		}
	}
}
#endif // WITH_EDITORONLY_DATA

#undef LOCTEXT_NAMESPACE

