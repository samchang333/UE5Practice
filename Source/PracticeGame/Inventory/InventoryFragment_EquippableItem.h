// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Inventory/PracticeInventoryItemDefinition.h"
#include "Templates/SubclassOf.h"

#include "InventoryFragment_EquippableItem.generated.h"

class UPracticeEquipmentDefinition;
class UObject;

UCLASS()
class UInventoryFragment_EquippableItem : public UPracticeInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=Practice)
	TSubclassOf<UPracticeEquipmentDefinition> EquipmentDefinition;
};
