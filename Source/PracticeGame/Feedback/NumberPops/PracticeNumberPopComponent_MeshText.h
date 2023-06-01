// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PracticeNumberPopComponent.h"

#include "PracticeNumberPopComponent_MeshText.generated.h"

class UPracticeDamagePopStyle;
class UMaterialInstanceDynamic;
class UObject;
class UStaticMesh;
class UStaticMeshComponent;

USTRUCT()
struct FPooledNumberPopComponentList
{
	GENERATED_BODY()

	UPROPERTY(transient)
	TArray<TObjectPtr<UStaticMeshComponent>> Components;
};

USTRUCT()
struct FLiveNumberPopEntry
{
	GENERATED_BODY()

	/** The component that is currently live */
	UPROPERTY(transient)
	TObjectPtr<UStaticMeshComponent> Component = nullptr;

	/** The pool this component will go into when released */
	FPooledNumberPopComponentList* Pool = nullptr;

	/** The world time that this component will be released to the pool */
	float ReleaseTime = 0.0f;

	FLiveNumberPopEntry()
	{}

	FLiveNumberPopEntry(UStaticMeshComponent* InComponent, FPooledNumberPopComponentList* InPool, float InReleaseTime)
		: Component(InComponent), Pool(InPool), ReleaseTime(InReleaseTime)
	{}
};

/** Struct that holds the info for a new damage number */
struct FTempNumberPopInfo
{
	UStaticMeshComponent* StaticMeshComponent = nullptr;

	TArray<UMaterialInstanceDynamic*> MeshMIDs;

	TArray<int32> DamageNumberArray;
};




UCLASS(Blueprintable)
class UPracticeNumberPopComponent_MeshText : public UPracticeNumberPopComponent
{
	GENERATED_BODY()

public:

	UPracticeNumberPopComponent_MeshText(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UPracticeNumberPopComponent interface
	virtual void AddNumberPop(const FPracticeNumberPopRequest& NewRequest) override;
	//~End of UPracticeNumberPopComponent interface

protected:
	void SetMaterialParameters(const FPracticeNumberPopRequest& Request, FTempNumberPopInfo& NewDamageNumberInfo, const FTransform& CameraTransform, const FVector& NumberLocation);

	FLinearColor DetermineColor(const FPracticeNumberPopRequest& Request) const;
	UStaticMesh* DetermineStaticMesh(const FPracticeNumberPopRequest& Request) const;


	/** Releases components back to the pool that have exceeded their lifespan */
	void ReleaseNextComponents();

	/** Style patterns to attempt to apply to the incoming number pops */
	UPROPERTY(EditDefaultsOnly, Category="Number Pop|Style")
	TArray<TObjectPtr<UPracticeDamagePopStyle>> Styles;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Number Pop|Style")
	float ComponentLifespan;

	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	float DistanceFromCameraBeforeDoublingSize;
	
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	float CriticalHitSizeMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Font")
	float FontXSize;

	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Font")
	float FontYSize;

	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Font")
	float SpacingPercentageForOnes;

	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	float NumberOfNumberRotations;

	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Material Bindings")
	FName SignDigitParameterName;

	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Material Bindings")
	FName ColorParameterName;

	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Material Bindings")
	FName AnimationLifespanParameterName;

	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Material Bindings")
	FName IsCriticalHitParameterName;

	/** Damage numbers by default are given a depth close to the camera in the material to make sure they are never occluded. This can be toggled off here, should only be 0/1. */
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Material Bindings")
	FName MoveToCameraParameterName;

	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Material Bindings")
	TArray<FName> PositionParameterNames;

	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Material Bindings")
	TArray<FName> ScaleRotationAngleParameterNames;

	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Material Bindings")
	TArray<FName> DurationParameterNames;

	UPROPERTY(Transient)
	TMap<TObjectPtr<UStaticMesh>, FPooledNumberPopComponentList> PooledComponentMap;

	UPROPERTY(transient)
	TArray<FLiveNumberPopEntry> LiveComponents;

	FTimerHandle ReleaseTimerHandle;
};
