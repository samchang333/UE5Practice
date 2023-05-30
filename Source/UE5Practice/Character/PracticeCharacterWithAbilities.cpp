// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeCharacterWithAbilities.h"

#include "AbilitySystem/Attributes/PracticeCombatSet.h"
#include "AbilitySystem/Attributes/PracticeHealthSet.h"
#include "AbilitySystem/PracticeAbilitySystemComponent.h"
#include "Async/TaskGraphInterfaces.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeCharacterWithAbilities)

APracticeCharacterWithAbilities::APracticeCharacterWithAbilities(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UPracticeAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	CreateDefaultSubobject<UPracticeHealthSet>(TEXT("HealthSet"));
	CreateDefaultSubobject<UPracticeCombatSet>(TEXT("CombatSet"));

	// AbilitySystemComponent needs to be updated at a high frequency.
	NetUpdateFrequency = 100.0f;
}

void APracticeCharacterWithAbilities::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

UAbilitySystemComponent* APracticeCharacterWithAbilities::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

