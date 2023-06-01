// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Character/PracticeCharacter.h"

#include "PracticeCharacterWithAbilities.generated.h"

class UAbilitySystemComponent;
class UPracticeAbilitySystemComponent;
class UObject;

// APracticeCharacter typically gets the ability system component from the possessing player state
// This represents a character with a self-contained ability system component.
UCLASS(Blueprintable)
class PRACTICEGAME_API APracticeCharacterWithAbilities : public APracticeCharacter
{
	GENERATED_BODY()

public:
	APracticeCharacterWithAbilities(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:

	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "Practice|PlayerState")
	TObjectPtr<UPracticeAbilitySystemComponent> AbilitySystemComponent;
};
