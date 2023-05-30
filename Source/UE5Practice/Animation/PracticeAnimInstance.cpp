// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeAnimInstance.h"
#include "AbilitySystemGlobals.h"
#include "Character/PracticeCharacter.h"
#include "Character/PracticeCharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeAnimInstance)


UPracticeAnimInstance::UPracticeAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPracticeAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);

	GameplayTagPropertyMap.Initialize(this, ASC);
}

#if WITH_EDITOR
EDataValidationResult UPracticeAnimInstance::IsDataValid(TArray<FText>& ValidationErrors)
{
	Super::IsDataValid(ValidationErrors);

	GameplayTagPropertyMap.IsDataValid(this, ValidationErrors);

	return ((ValidationErrors.Num() > 0) ? EDataValidationResult::Invalid : EDataValidationResult::Valid);
}
#endif // WITH_EDITOR

void UPracticeAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AActor* OwningActor = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			InitializeWithAbilitySystem(ASC);
		}
	}
}

void UPracticeAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const APracticeCharacter* Character = Cast<APracticeCharacter>(GetOwningActor());
	if (!Character)
	{
		return;
	}

	UPracticeCharacterMovementComponent* CharMoveComp = CastChecked<UPracticeCharacterMovementComponent>(Character->GetCharacterMovement());
	const FPracticeCharacterGroundInfo& GroundInfo = CharMoveComp->GetGroundInfo();
	GroundDistance = GroundInfo.GroundDistance;
}

