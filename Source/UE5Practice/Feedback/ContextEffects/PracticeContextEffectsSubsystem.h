// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"

#include "PracticeContextEffectsSubsystem.generated.h"

enum EPhysicalSurface : int;

class AActor;
class UAudioComponent;
class UPracticeContextEffectsLibrary;
class UNiagaraComponent;
class USceneComponent;
struct FFrame;
struct FGameplayTag;
struct FGameplayTagContainer;

/**
 *
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "PracticeContextEffects"))
class UE5PRACTICE_API UPracticeContextEffectsSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	//
	UPROPERTY(config, EditAnywhere)
	TMap<TEnumAsByte<EPhysicalSurface>, FGameplayTag> SurfaceTypeToContextMap;
};

/**
 *
 */
UCLASS()
class UE5PRACTICE_API UPracticeContextEffectsSet : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TSet<TObjectPtr<UPracticeContextEffectsLibrary>> PracticeContextEffectsLibraries;
};


/**
 * 
 */
UCLASS()
class UE5PRACTICE_API UPracticeContextEffectsSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	/** */
	UFUNCTION(BlueprintCallable, Category = "ContextEffects")
	void SpawnContextEffects(
		const AActor* SpawningActor
		, USceneComponent* AttachToComponent
		, const FName AttachPoint
		, const FVector LocationOffset
		, const FRotator RotationOffset
		, FGameplayTag Effect
		, FGameplayTagContainer Contexts
		, TArray<UAudioComponent*>& AudioOut
		, TArray<UNiagaraComponent*>& NiagaraOut
		, FVector VFXScale = FVector(1)
		, float AudioVolume = 1
		, float AudioPitch = 1);

	/** */
	UFUNCTION(BlueprintCallable, Category = "ContextEffects")
	bool GetContextFromSurfaceType(TEnumAsByte<EPhysicalSurface> PhysicalSurface, FGameplayTag& Context);

	/** */
	UFUNCTION(BlueprintCallable, Category = "ContextEffects")
	void LoadAndAddContextEffectsLibraries(AActor* OwningActor, TSet<TSoftObjectPtr<UPracticeContextEffectsLibrary>> ContextEffectsLibraries);

	/** */
	UFUNCTION(BlueprintCallable, Category = "ContextEffects")
	void UnloadAndRemoveContextEffectsLibraries(AActor* OwningActor);

private:

	UPROPERTY(Transient)
	TMap<TObjectPtr<AActor>, TObjectPtr<UPracticeContextEffectsSet>> ActiveActorEffectsMap;

};
