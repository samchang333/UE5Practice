// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "PracticeContextEffectsInterface.h"

#include "PracticeContextEffectComponent.generated.h"

namespace EEndPlayReason { enum Type : int; }

class UAnimSequenceBase;
class UAudioComponent;
class UPracticeContextEffectsLibrary;
class UNiagaraComponent;
class UObject;
class USceneComponent;
struct FFrame;
struct FHitResult;

UCLASS( ClassGroup=(Custom), hidecategories = (Variable, Tags, ComponentTick, ComponentReplication, Activation, Cooking, AssetUserData, Collision), CollapseCategories, meta=(BlueprintSpawnableComponent) )
class UE5PRACTICE_API UPracticeContextEffectComponent : public UActorComponent, public IPracticeContextEffectsInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPracticeContextEffectComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called when the game ends
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// AnimMotionEffect Implementation
	UFUNCTION(BlueprintCallable)
	virtual void AnimMotionEffect_Implementation(const FName Bone, const FGameplayTag MotionEffect, USceneComponent* StaticMeshComponent,
		const FVector LocationOffset, const FRotator RotationOffset, const UAnimSequenceBase* AnimationSequence,
		const bool bHitSuccess, const FHitResult HitResult, FGameplayTagContainer Contexts,
		FVector VFXScale = FVector(1), float AudioVolume = 1, float AudioPitch = 1) override;

	// Auto-Convert Physical Surface from Trace Result to Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bConvertPhysicalSurfaceToContext = true;

	// Default Contexts
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer DefaultEffectContexts;

	// Default Libraries for this Actor
	UPROPERTY(EditAnywhere)
	TSet<TSoftObjectPtr<UPracticeContextEffectsLibrary>> DefaultContextEffectsLibraries;

	UFUNCTION(BlueprintCallable)
	void UpdateEffectContexts(FGameplayTagContainer NewEffectContexts);

	UFUNCTION(BlueprintCallable)
	void UpdateLibraries(TSet<TSoftObjectPtr<UPracticeContextEffectsLibrary>> NewContextEffectsLibraries);

private:
	UPROPERTY(Transient)
	FGameplayTagContainer CurrentContexts;

	UPROPERTY(Transient)
	TSet<TSoftObjectPtr<UPracticeContextEffectsLibrary>> CurrentContextEffectsLibraries;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UAudioComponent>> ActiveAudioComponents;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UNiagaraComponent>> ActiveNiagaraComponents;
};
