// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeInventoryItemDefinition.h"

#include "Templates/SubclassOf.h"
#include "UObject/ObjectPtr.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeInventoryItemDefinition)

//////////////////////////////////////////////////////////////////////
// UPracticeInventoryItemDefinition

UPracticeInventoryItemDefinition::UPracticeInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UPracticeInventoryItemFragment* UPracticeInventoryItemDefinition::FindFragmentByClass(TSubclassOf<UPracticeInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (UPracticeInventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////
// UPracticeInventoryItemDefinition

const UPracticeInventoryItemFragment* UPracticeInventoryFunctionLibrary::FindItemDefinitionFragment(TSubclassOf<UPracticeInventoryItemDefinition> ItemDef, TSubclassOf<UPracticeInventoryItemFragment> FragmentClass)
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UPracticeInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}

