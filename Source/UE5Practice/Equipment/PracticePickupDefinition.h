// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"

#include "PracticePickupDefinition.generated.h"

class UPracticeInventoryItemDefinition;
class UNiagaraSystem;
class UObject;
class USoundBase;
class UStaticMesh;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Const, Meta = (DisplayName = "Practice Pickup Data", ShortTooltip = "Data asset used to configure a pickup."))
class UE5PRACTICE_API UPracticePickupDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:

	//Defines the pickup's actors to spawn, abilities to grant, and tags to add
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Practice|Pickup|Equipment")
	TSubclassOf<UPracticeInventoryItemDefinition> InventoryItemDefinition;

	//Visual representation of the pickup
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Practice|Pickup|Mesh")
	TObjectPtr<UStaticMesh> DisplayMesh;

	//Cool down time between pickups in seconds
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Practice|Pickup")
	int32 SpawnCoolDownSeconds;

	//Sound to play when picked up
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Practice|Pickup")
	TObjectPtr<USoundBase> PickedUpSound;

	//Sound to play when pickup is respawned
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Practice|Pickup")
	TObjectPtr<USoundBase> RespawnedSound;

	//Particle FX to play when picked up
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Practice|Pickup")
	TObjectPtr<UNiagaraSystem> PickedUpEffect;

	//Particle FX to play when pickup is respawned
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Practice|Pickup")
	TObjectPtr<UNiagaraSystem> RespawnedEffect;
};


UCLASS(Blueprintable, BlueprintType, Const, Meta = (DisplayName = "Practice Weapon Pickup Data", ShortTooltip = "Data asset used to configure a weapon pickup."))
class UE5PRACTICE_API UPracticeWeaponPickupDefinition : public UPracticePickupDefinition
{
	GENERATED_BODY()

public:

	//Sets the height of the display mesh above the Weapon spawner
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Practice|Pickup|Mesh")
	FVector WeaponMeshOffset;

	//Sets the height of the display mesh above the Weapon spawner
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Practice|Pickup|Mesh")
	FVector WeaponMeshScale = FVector(1.0f, 1.0f, 1.0f);
};
