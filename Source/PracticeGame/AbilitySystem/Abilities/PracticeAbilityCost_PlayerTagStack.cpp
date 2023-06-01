// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeAbilityCost_PlayerTagStack.h"

#include "GameFramework/Controller.h"
#include "PracticeGameplayAbility.h"
#include "Player/PracticePlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeAbilityCost_PlayerTagStack)

UPracticeAbilityCost_PlayerTagStack::UPracticeAbilityCost_PlayerTagStack()
{
	Quantity.SetValue(1.0f);
}

bool UPracticeAbilityCost_PlayerTagStack::CheckCost(const UPracticeGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (AController* PC = Ability->GetControllerFromActorInfo())
	{
		if (APracticePlayerState* PS = Cast<APracticePlayerState>(PC->PlayerState))
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

			const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

			return PS->GetStatTagStackCount(Tag) >= NumStacks;
		}
	}
	return false;
}

void UPracticeAbilityCost_PlayerTagStack::ApplyCost(const UPracticeGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo->IsNetAuthority())
	{
		if (AController* PC = Ability->GetControllerFromActorInfo())
		{
			if (APracticePlayerState* PS = Cast<APracticePlayerState>(PC->PlayerState))
			{
				const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

				const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
				const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

				PS->RemoveStatTagStack(Tag, NumStacks);
			}
		}
	}
}

