// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeCombatSet.h"

#include "AbilitySystem/Attributes/PracticeAttributeSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeCombatSet)

class FLifetimeProperty;


UPracticeCombatSet::UPracticeCombatSet()
	: BaseDamage(0.0f)
	, BaseHeal(0.0f)
{
}

void UPracticeCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPracticeCombatSet, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPracticeCombatSet, BaseHeal, COND_OwnerOnly, REPNOTIFY_Always);
}

void UPracticeCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPracticeCombatSet, BaseDamage, OldValue);
}

void UPracticeCombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPracticeCombatSet, BaseHeal, OldValue);
}

