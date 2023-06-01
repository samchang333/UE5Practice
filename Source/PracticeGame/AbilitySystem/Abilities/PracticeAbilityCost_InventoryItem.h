// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PracticeAbilityCost.h"
#include "ScalableFloat.h"
#include "Templates/SubclassOf.h"

#include "PracticeAbilityCost_InventoryItem.generated.h"

struct FGameplayAbilityActivationInfo;
struct FGameplayAbilitySpecHandle;

class UPracticeGameplayAbility;
class UPracticeInventoryItemDefinition;
class UObject;
struct FGameplayAbilityActorInfo;
struct FGameplayTagContainer;

/**
 * Represents a cost that requires expending a quantity of an inventory item
 */
UCLASS(meta=(DisplayName="Inventory Item"))
class UPracticeAbilityCost_InventoryItem : public UPracticeAbilityCost
{
	GENERATED_BODY()

public:
	UPracticeAbilityCost_InventoryItem();

	//~UPracticeAbilityCost interface
	virtual bool CheckCost(const UPracticeGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const UPracticeGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	//~End of UPracticeAbilityCost interface

protected:
	/** How much of the item to spend (keyed on ability level) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AbilityCost)
	FScalableFloat Quantity;

	/** Which item to consume */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AbilityCost)
	TSubclassOf<UPracticeInventoryItemDefinition> ItemDefinition;
};
