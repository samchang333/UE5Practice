// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystem/Abilities/PracticeGameplayAbility.h"

#include "PracticeGameplayAbility_FromEquipment.generated.h"

class UPracticeEquipmentInstance;
class UPracticeInventoryItemInstance;

/**
 * UPracticeGameplayAbility_FromEquipment
 *
 * An ability granted by and associated with an equipment instance
 */
UCLASS()
class UPracticeGameplayAbility_FromEquipment : public UPracticeGameplayAbility
{
	GENERATED_BODY()

public:

	UPracticeGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Practice|Ability")
	UPracticeEquipmentInstance* GetAssociatedEquipment() const;

	UFUNCTION(BlueprintCallable, Category = "Practice|Ability")
	UPracticeInventoryItemInstance* GetAssociatedItem() const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#endif

};
