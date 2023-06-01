// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemInterface.h"
#include "ModularGameState.h"

#include "PracticeGameState.generated.h"

struct FPracticeVerbMessage;

class APlayerState;
class UAbilitySystemComponent;
class UPracticeAbilitySystemComponent;
class UPracticeExperienceManagerComponent;
class UObject;
struct FFrame;

/**
 * APracticeGameState
 *
 *	The base game state class used by this project.
 */
UCLASS(Config = Game)
class PRACTICEGAME_API APracticeGameState : public AModularGameStateBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	APracticeGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	float GetServerFPS() const { return ServerFPS; }

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor interface

	//~AGameStateBase interface
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	virtual void SeamlessTravelTransitionCheckpoint(bool bToTransitionMap) override;
	//~End of AGameStateBase interface

	//~IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface

	UFUNCTION(BlueprintCallable, Category = "Practice|GameState")
	UPracticeAbilitySystemComponent* GetPracticeAbilitySystemComponent() const { return AbilitySystemComponent; }

	// Send a message that all clients will (probably) get
	// (use only for client notifications like eliminations, server join messages, etc... that can handle being lost)
	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category = "Practice|GameState")
	void MulticastMessageToClients(const FPracticeVerbMessage Message);

	// Send a message that all clients will be guaranteed to get
	// (use only for client notifications that cannot handle being lost)
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Practice|GameState")
	void MulticastReliableMessageToClients(const FPracticeVerbMessage Message);

private:
	UPROPERTY()
	TObjectPtr<UPracticeExperienceManagerComponent> ExperienceManagerComponent;

	// The ability system component subobject for game-wide things (primarily gameplay cues)
	UPROPERTY(VisibleAnywhere, Category = "Practice|GameState")
	TObjectPtr<UPracticeAbilitySystemComponent> AbilitySystemComponent;


protected:

	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(Replicated)
	float ServerFPS;
};
