// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonLocalPlayer.h"
#include "Teams/PracticeTeamAgentInterface.h"

#include "PracticeLocalPlayer.generated.h"

struct FGenericTeamId;

class APlayerController;
class UInputMappingContext;
class UPracticeSettingsLocal;
class UPracticeSettingsShared;
class UObject;
class UWorld;
struct FFrame;
struct FSwapAudioOutputResult;

/**
 * UPracticeLocalPlayer
 */
UCLASS()
class UE5PRACTICE_API UPracticeLocalPlayer : public UCommonLocalPlayer, public IPracticeTeamAgentInterface
{
	GENERATED_BODY()

public:

	UPracticeLocalPlayer();

	//~UObject interface
	virtual void PostInitProperties() override;
	//~End of UObject interface

	//~UPlayer interface
	virtual void SwitchController(class APlayerController* PC) override;
	//~End of UPlayer interface

	//~ULocalPlayer interface
	virtual bool SpawnPlayActor(const FString& URL, FString& OutError, UWorld* InWorld) override;
	virtual void InitOnlineSession() override;
	//~End of ULocalPlayer interface

	//~IPracticeTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOnPracticeTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of IPracticeTeamAgentInterface interface

public:
	UFUNCTION()
	UPracticeSettingsLocal* GetLocalSettings() const;

	UFUNCTION()
	UPracticeSettingsShared* GetSharedSettings() const;

protected:
	void OnAudioOutputDeviceChanged(const FString& InAudioOutputDeviceId);
	
	UFUNCTION()
	void OnCompletedAudioDeviceSwap(const FSwapAudioOutputResult& SwapResult);

private:
	void OnPlayerControllerChanged(APlayerController* NewController);

	UFUNCTION()
	void OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

private:
	UPROPERTY(Transient)
	mutable TObjectPtr<UPracticeSettingsShared> SharedSettings;

	UPROPERTY(Transient)
	mutable TObjectPtr<const UInputMappingContext> InputMappingContext;

	UPROPERTY()
	FOnPracticeTeamIndexChangedDelegate OnTeamChangedDelegate;

	UPROPERTY()
	TWeakObjectPtr<APlayerController> LastBoundPC;
};
