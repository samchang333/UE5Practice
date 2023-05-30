// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "PracticeInventoryItemDefinition.generated.h"

template <typename T> class TSubclassOf;

class UPracticeInventoryItemInstance;
struct FFrame;

//////////////////////////////////////////////////////////////////////

// Represents a fragment of an item definition
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class UE5PRACTICE_API UPracticeInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UPracticeInventoryItemInstance* Instance) const {}
};

//////////////////////////////////////////////////////////////////////

/**
 * UPracticeInventoryItemDefinition
 */
UCLASS(Blueprintable, Const, Abstract)
class UPracticeInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UPracticeInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
	TArray<TObjectPtr<UPracticeInventoryItemFragment>> Fragments;

public:
	const UPracticeInventoryItemFragment* FindFragmentByClass(TSubclassOf<UPracticeInventoryItemFragment> FragmentClass) const;
};

//@TODO: Make into a subsystem instead?
UCLASS()
class UPracticeInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType=FragmentClass))
	static const UPracticeInventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<UPracticeInventoryItemDefinition> ItemDef, TSubclassOf<UPracticeInventoryItemFragment> FragmentClass);
};
