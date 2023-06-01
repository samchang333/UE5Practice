// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "PracticeHealExecution.generated.h"

class UObject;


/**
 * UPracticeHealExecution
 *
 *	Execution used by gameplay effects to apply healing to the health attributes.
 */
UCLASS()
class UPracticeHealExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UPracticeHealExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
