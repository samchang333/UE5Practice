// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeGameplayAbility_FromEquipment.h"
#include "PracticeEquipmentInstance.h"
#include "Inventory/PracticeInventoryItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeGameplayAbility_FromEquipment)

UPracticeGameplayAbility_FromEquipment::UPracticeGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UPracticeEquipmentInstance* UPracticeGameplayAbility_FromEquipment::GetAssociatedEquipment() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<UPracticeEquipmentInstance>(Spec->SourceObject.Get());
	}

	return nullptr;
}

UPracticeInventoryItemInstance* UPracticeGameplayAbility_FromEquipment::GetAssociatedItem() const
{
	if (UPracticeEquipmentInstance* Equipment = GetAssociatedEquipment())
	{
		return Cast<UPracticeInventoryItemInstance>(Equipment->GetInstigator());
	}
	return nullptr;
}


#if WITH_EDITOR
EDataValidationResult UPracticeGameplayAbility_FromEquipment::IsDataValid(TArray<FText>& ValidationErrors)
{
	EDataValidationResult Result = Super::IsDataValid(ValidationErrors);

	if (InstancingPolicy == EGameplayAbilityInstancingPolicy::NonInstanced)
	{
		ValidationErrors.Add(NSLOCTEXT("Practice", "EquipmentAbilityMustBeInstanced", "Equipment ability must be instanced"));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif
