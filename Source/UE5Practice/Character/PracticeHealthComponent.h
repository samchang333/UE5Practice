// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/GameFrameworkComponent.h"

#include "PracticeHealthComponent.generated.h"

class UPracticeHealthComponent;

class UPracticeAbilitySystemComponent;
class UPracticeHealthSet;
class UObject;
struct FFrame;
struct FGameplayEffectSpec;
struct FOnAttributeChangeData;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPracticeHealth_DeathEvent, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPracticeHealth_AttributeChanged, UPracticeHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);


/**
 * EPracticeDeathState
 *
 *	Defines current state of death.
 */
UENUM(BlueprintType)
enum class EPracticeDeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};


/**
 * UPracticeHealthComponent
 *
 *	An actor component used to handle anything related to health.
 */
UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class UE5PRACTICE_API UPracticeHealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()

public:

	UPracticeHealthComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the health component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "Practice|Health")
	static UPracticeHealthComponent* FindHealthComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UPracticeHealthComponent>() : nullptr); }

	// Initialize the component using an ability system component.
	UFUNCTION(BlueprintCallable, Category = "Practice|Health")
	void InitializeWithAbilitySystem(UPracticeAbilitySystemComponent* InASC);

	// Uninitialize the component, clearing any references to the ability system.
	UFUNCTION(BlueprintCallable, Category = "Practice|Health")
	void UninitializeFromAbilitySystem();

	// Returns the current health value.
	UFUNCTION(BlueprintCallable, Category = "Practice|Health")
	float GetHealth() const;

	// Returns the current maximum health value.
	UFUNCTION(BlueprintCallable, Category = "Practice|Health")
	float GetMaxHealth() const;

	// Returns the current health in the range [0.0, 1.0].
	UFUNCTION(BlueprintCallable, Category = "Practice|Health")
	float GetHealthNormalized() const;

	UFUNCTION(BlueprintCallable, Category = "Practice|Health")
	EPracticeDeathState GetDeathState() const { return DeathState; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Practice|Health", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadOrDying() const { return (DeathState > EPracticeDeathState::NotDead); }

	// Begins the death sequence for the owner.
	virtual void StartDeath();

	// Ends the death sequence for the owner.
	virtual void FinishDeath();

	// Applies enough damage to kill the owner.
	virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);

public:

	// Delegate fired when the health value has changed.
	UPROPERTY(BlueprintAssignable)
	FPracticeHealth_AttributeChanged OnHealthChanged;

	// Delegate fired when the max health value has changed.
	UPROPERTY(BlueprintAssignable)
	FPracticeHealth_AttributeChanged OnMaxHealthChanged;

	// Delegate fired when the death sequence has started.
	UPROPERTY(BlueprintAssignable)
	FPracticeHealth_DeathEvent OnDeathStarted;

	// Delegate fired when the death sequence has finished.
	UPROPERTY(BlueprintAssignable)
	FPracticeHealth_DeathEvent OnDeathFinished;

protected:

	virtual void OnUnregister() override;

	void ClearGameplayTags();

	virtual void HandleHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

	UFUNCTION()
	virtual void OnRep_DeathState(EPracticeDeathState OldDeathState);

protected:

	// Ability system used by this component.
	UPROPERTY()
	TObjectPtr<UPracticeAbilitySystemComponent> AbilitySystemComponent;

	// Health set used by this component.
	UPROPERTY()
	TObjectPtr<const UPracticeHealthSet> HealthSet;

	// Replicated state used to handle dying.
	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	EPracticeDeathState DeathState;
};
