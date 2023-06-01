// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeGameplayAbility_Jump.h"

#include "AbilitySystem/Abilities/PracticeGameplayAbility.h"
#include "Character/PracticeCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeGameplayAbility_Jump)

struct FGameplayTagContainer;


UPracticeGameplayAbility_Jump::UPracticeGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UPracticeGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const APracticeCharacter* PracticeCharacter = Cast<APracticeCharacter>(ActorInfo->AvatarActor.Get());
	if (!PracticeCharacter || !PracticeCharacter->CanJump())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UPracticeGameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Stop jumping in case the ability blueprint doesn't call it.
	CharacterJumpStop();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UPracticeGameplayAbility_Jump::CharacterJumpStart()
{
	if (APracticeCharacter* PracticeCharacter = GetPracticeCharacterFromActorInfo())
	{
		if (PracticeCharacter->IsLocallyControlled() && !PracticeCharacter->bPressedJump)
		{
			PracticeCharacter->UnCrouch();
			PracticeCharacter->Jump();
		}
	}
}

void UPracticeGameplayAbility_Jump::CharacterJumpStop()
{
	if (APracticeCharacter* PracticeCharacter = GetPracticeCharacterFromActorInfo())
	{
		if (PracticeCharacter->IsLocallyControlled() && PracticeCharacter->bPressedJump)
		{
			PracticeCharacter->StopJumping();
		}
	}
}

