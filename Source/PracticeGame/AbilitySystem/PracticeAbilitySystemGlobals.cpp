// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeAbilitySystemGlobals.h"

#include "PracticeGameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeAbilitySystemGlobals)

struct FGameplayEffectContext;

UPracticeAbilitySystemGlobals::UPracticeAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FGameplayEffectContext* UPracticeAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FPracticeGameplayEffectContext();
}

