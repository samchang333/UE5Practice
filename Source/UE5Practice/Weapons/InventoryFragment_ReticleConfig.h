// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Inventory/PracticeInventoryItemDefinition.h"

#include "InventoryFragment_ReticleConfig.generated.h"

class UPracticeReticleWidgetBase;
class UObject;

UCLASS()
class UInventoryFragment_ReticleConfig : public UPracticeInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Reticle)
	TArray<TSubclassOf<UPracticeReticleWidgetBase>> ReticleWidgets;
};
