// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeGameplayAbility_Death.h"

#include "AbilitySystem/Abilities/PracticeGameplayAbility.h"
#include "AbilitySystem/PracticeAbilitySystemComponent.h"
#include "Character/PracticeHealthComponent.h"
#include "PracticeGameplayTags.h"
#include "PracticeLogChannels.h"
#include "Trace/Trace.inl"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeGameplayAbility_Death)

UPracticeGameplayAbility_Death::UPracticeGameplayAbility_Death(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	bAutoStartDeath = true;

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		// Add the ability trigger tag as default to the CDO.
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = PracticeGameplayTags::GameplayEvent_Death;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UPracticeGameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	check(ActorInfo);

	UPracticeAbilitySystemComponent* PracticeASC = CastChecked<UPracticeAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

	FGameplayTagContainer AbilityTypesToIgnore;
	AbilityTypesToIgnore.AddTag(PracticeGameplayTags::Ability_Behavior_SurvivesDeath);

	// Cancel all abilities and block others from starting.
	PracticeASC->CancelAbilities(nullptr, &AbilityTypesToIgnore, this);

	SetCanBeCanceled(false);

	if (!ChangeActivationGroup(EPracticeAbilityActivationGroup::Exclusive_Blocking))
	{
		UE_LOG(LogPracticeAbilitySystem, Error, TEXT("UPracticeGameplayAbility_Death::ActivateAbility: Ability [%s] failed to change activation group to blocking."), *GetName());
	}

	if (bAutoStartDeath)
	{
		StartDeath();
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UPracticeGameplayAbility_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	check(ActorInfo);

	// Always try to finish the death when the ability ends in case the ability doesn't.
	// This won't do anything if the death hasn't been started.
	FinishDeath();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UPracticeGameplayAbility_Death::StartDeath()
{
	if (UPracticeHealthComponent* HealthComponent = UPracticeHealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == EPracticeDeathState::NotDead)
		{
			HealthComponent->StartDeath();
		}
	}
}

void UPracticeGameplayAbility_Death::FinishDeath()
{
	if (UPracticeHealthComponent* HealthComponent = UPracticeHealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == EPracticeDeathState::DeathStarted)
		{
			HealthComponent->FinishDeath();
		}
	}
}

