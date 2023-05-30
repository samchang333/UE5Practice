// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeEquipmentDefinition.h"
#include "PracticeEquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeEquipmentDefinition)

UPracticeEquipmentDefinition::UPracticeEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstanceType = UPracticeEquipmentInstance::StaticClass();
}

