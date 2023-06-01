// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/PracticeLocalPlayer.h"

#include "AudioMixerBlueprintLibrary.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Settings/PracticeSettingsLocal.h"
#include "Settings/PracticeSettingsShared.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeLocalPlayer)

class UObject;

UPracticeLocalPlayer::UPracticeLocalPlayer()
{
}

void UPracticeLocalPlayer::PostInitProperties()
{
	Super::PostInitProperties();

	if (UPracticeSettingsLocal* LocalSettings = GetLocalSettings())
	{
		LocalSettings->OnAudioOutputDeviceChanged.AddUObject(this, &UPracticeLocalPlayer::OnAudioOutputDeviceChanged);
	}
}

void UPracticeLocalPlayer::SwitchController(class APlayerController* PC)
{
	Super::SwitchController(PC);

	OnPlayerControllerChanged(PlayerController);
}

bool UPracticeLocalPlayer::SpawnPlayActor(const FString& URL, FString& OutError, UWorld* InWorld)
{
	const bool bResult = Super::SpawnPlayActor(URL, OutError, InWorld);

	OnPlayerControllerChanged(PlayerController);

	return bResult;
}

void UPracticeLocalPlayer::InitOnlineSession()
{
	OnPlayerControllerChanged(PlayerController);

	Super::InitOnlineSession();
}

void UPracticeLocalPlayer::OnPlayerControllerChanged(APlayerController* NewController)
{
	// Stop listening for changes from the old controller
	FGenericTeamId OldTeamID = FGenericTeamId::NoTeam;
	if (IPracticeTeamAgentInterface* ControllerAsTeamProvider = Cast<IPracticeTeamAgentInterface>(LastBoundPC.Get()))
	{
		OldTeamID = ControllerAsTeamProvider->GetGenericTeamId();
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().RemoveAll(this);
	}

	// Grab the current team ID and listen for future changes
	FGenericTeamId NewTeamID = FGenericTeamId::NoTeam;
	if (IPracticeTeamAgentInterface* ControllerAsTeamProvider = Cast<IPracticeTeamAgentInterface>(NewController))
	{
		NewTeamID = ControllerAsTeamProvider->GetGenericTeamId();
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnControllerChangedTeam);
		LastBoundPC = NewController;
	}

	ConditionalBroadcastTeamChanged(this, OldTeamID, NewTeamID);
}

void UPracticeLocalPlayer::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	// Do nothing, we merely observe the team of our associated player controller
}

FGenericTeamId UPracticeLocalPlayer::GetGenericTeamId() const
{
	if (IPracticeTeamAgentInterface* ControllerAsTeamProvider = Cast<IPracticeTeamAgentInterface>(PlayerController))
	{
		return ControllerAsTeamProvider->GetGenericTeamId();
	}
	else
	{
		return FGenericTeamId::NoTeam;
	}
}

FOnPracticeTeamIndexChangedDelegate* UPracticeLocalPlayer::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

UPracticeSettingsLocal* UPracticeLocalPlayer::GetLocalSettings() const
{
	return UPracticeSettingsLocal::Get();
}

UPracticeSettingsShared* UPracticeLocalPlayer::GetSharedSettings() const
{
	if (!SharedSettings)
	{
		SharedSettings = UPracticeSettingsShared::LoadOrCreateSettings(this);
	}

	return SharedSettings;
}

void UPracticeLocalPlayer::OnAudioOutputDeviceChanged(const FString& InAudioOutputDeviceId)
{
	FOnCompletedDeviceSwap DevicesSwappedCallback;
	DevicesSwappedCallback.BindUFunction(this, FName("OnCompletedAudioDeviceSwap"));
	UAudioMixerBlueprintLibrary::SwapAudioOutputDevice(GetWorld(), InAudioOutputDeviceId, DevicesSwappedCallback);
}

void UPracticeLocalPlayer::OnCompletedAudioDeviceSwap(const FSwapAudioOutputResult& SwapResult)
{
	if (SwapResult.Result == ESwapAudioOutputDeviceResultState::Failure)
	{
	}
}

void UPracticeLocalPlayer::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	ConditionalBroadcastTeamChanged(this, IntegerToGenericTeamId(OldTeam), IntegerToGenericTeamId(NewTeam));
}

