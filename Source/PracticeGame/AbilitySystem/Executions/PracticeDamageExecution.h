// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "PracticeDamageExecution.generated.h"

class UObject;


/**
 * UPracticeDamageExecution
 *
 *	Execution used by gameplay effects to apply damage to the health attributes.
 */
UCLASS()
class UPracticeDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UPracticeDamageExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
