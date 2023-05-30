// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeAttributeSet.h"

#include "AbilitySystem/PracticeAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeAttributeSet)

class UWorld;


UPracticeAttributeSet::UPracticeAttributeSet()
{
}

UWorld* UPracticeAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

UPracticeAbilitySystemComponent* UPracticeAttributeSet::GetPracticeAbilitySystemComponent() const
{
	return Cast<UPracticeAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

