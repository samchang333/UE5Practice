// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"

#include "PracticePawnData.generated.h"

class APawn;
class UPracticeAbilitySet;
class UPracticeAbilityTagRelationshipMapping;
class UPracticeCameraMode;
class UPracticeInputConfig;
class UObject;


/**
 * UPracticePawnData
 *
 *	Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Practice Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class UE5PRACTICE_API UPracticePawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPracticePawnData(const FObjectInitializer& ObjectInitializer);

public:

	// Class to instantiate for this pawn (should usually derive from APracticePawn or APracticeCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Practice|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Practice|Abilities")
	TArray<TObjectPtr<UPracticeAbilitySet>> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Practice|Abilities")
	TObjectPtr<UPracticeAbilityTagRelationshipMapping> TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Practice|Input")
	TObjectPtr<UPracticeInputConfig> InputConfig;

	// Default camera mode used by player controlled pawns.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Practice|Camera")
	TSubclassOf<UPracticeCameraMode> DefaultCameraMode;
};
