// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PracticeGameplayAbility.h"

#include "PracticeGameplayAbility_Jump.generated.h"

class UObject;
struct FFrame;
struct FGameplayAbilityActorInfo;
struct FGameplayTagContainer;


/**
 * UPracticeGameplayAbility_Jump
 *
 *	Gameplay ability used for character jumping.
 */
UCLASS(Abstract)
class UPracticeGameplayAbility_Jump : public UPracticeGameplayAbility
{
	GENERATED_BODY()

public:

	UPracticeGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, Category = "Practice|Ability")
	void CharacterJumpStart();

	UFUNCTION(BlueprintCallable, Category = "Practice|Ability")
	void CharacterJumpStop();
};
