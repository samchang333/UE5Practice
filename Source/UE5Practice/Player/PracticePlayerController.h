// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Camera/PracticeCameraAssistInterface.h"
#include "CommonPlayerController.h"
#include "Teams/PracticeTeamAgentInterface.h"

#include "PracticePlayerController.generated.h"

struct FGenericTeamId;

class APracticeHUD;
class APracticePlayerState;
class APawn;
class APlayerState;
class FPrimitiveComponentId;
class IInputInterface;
class UPracticeAbilitySystemComponent;
class UPracticeSettingsShared;
class UObject;
class UPlayer;
struct FFrame;

/**
 * APracticePlayerController
 *
 *	The base player controller class used by this project.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base player controller class used by this project."))
class UE5PRACTICE_API APracticePlayerController : public ACommonPlayerController, public IPracticeCameraAssistInterface, public IPracticeTeamAgentInterface
{
	GENERATED_BODY()

public:

	APracticePlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Practice|PlayerController")
	APracticePlayerState* GetPracticePlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "Practice|PlayerController")
	UPracticeAbilitySystemComponent* GetPracticeAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Practice|PlayerController")
	APracticeHUD* GetPracticeHUD() const;

	// Run a cheat command on the server.
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCheat(const FString& Msg);

	// Run a cheat command on the server for all players.
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCheatAll(const FString& Msg);

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor interface

	//~AController interface
	virtual void OnUnPossess() override;
	//~End of AController interface

	//~APlayerController interface
	virtual void ReceivedPlayer() override;
	virtual void PlayerTick(float DeltaTime) override;
	//~End of APlayerController interface

	//~IPracticeCameraAssistInterface interface
	virtual void OnCameraPenetratingTarget() override;
	//~End of IPracticeCameraAssistInterface interface

	//~ACommonPlayerController interface
	virtual void OnPossess(APawn* InPawn) override;
	//~End of ACommonPlayerController interface
	
	//~IPracticeTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOnPracticeTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of IPracticeTeamAgentInterface interface

	UFUNCTION(BlueprintCallable, Category = "Practice|Character")
	void SetIsAutoRunning(const bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Practice|Character")
	bool GetIsAutoRunning() const;

private:
	UPROPERTY()
	FOnPracticeTeamIndexChangedDelegate OnTeamChangedDelegate;

	UPROPERTY()
	TObjectPtr<APlayerState> LastSeenPlayerState;

private:
	UFUNCTION()
	void OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

protected:
	// Called when the player state is set or cleared
	virtual void OnPlayerStateChanged();

private:
	void BroadcastOnPlayerStateChanged();

protected:
	//~AController interface
	virtual void InitPlayerState() override;
	virtual void CleanupPlayerState() override;
	virtual void OnRep_PlayerState() override;
	//~End of AController interface

	//~APlayerController interface
	virtual void SetPlayer(UPlayer* InPlayer) override;
	virtual void AddCheats(bool bForce) override;

	virtual void UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId) override;
	virtual void UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;

	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface

	void OnSettingsChanged(UPracticeSettingsShared* Settings);
	
	void OnStartAutoRun();
	void OnEndAutoRun();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartAutoRun"))
	void K2_OnStartAutoRun();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnEndAutoRun"))
	void K2_OnEndAutoRun();

	bool bHideViewTargetPawnNextFrame = false;
};


// A player controller used for replay capture and playback
UCLASS()
class APracticeReplayPlayerController : public APracticePlayerController
{
	GENERATED_BODY()

	virtual void SetPlayer(UPlayer* InPlayer) override;
};
