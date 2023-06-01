// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeInputComponent.h"

#include "EnhancedInputSubsystems.h"
#include "Player/PracticeLocalPlayer.h"
#include "Settings/PracticeSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeInputComponent)

class UPracticeInputConfig;

UPracticeInputComponent::UPracticeInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UPracticeInputComponent::AddInputMappings(const UPracticeInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	UPracticeLocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer<UPracticeLocalPlayer>();
	check(LocalPlayer);

	// Add any registered input mappings from the settings!
	if (UPracticeSettingsLocal* LocalSettings = UPracticeSettingsLocal::Get())
	{	
		// Tell enhanced input about any custom keymappings that the player may have customized
		for (const TPair<FName, FKey>& Pair : LocalSettings->GetCustomPlayerInputConfig())
		{
			if (Pair.Key != NAME_None && Pair.Value.IsValid())
			{
				InputSubsystem->AddPlayerMappedKeyInSlot(Pair.Key, Pair.Value);
			}
		}
	}
}

void UPracticeInputComponent::RemoveInputMappings(const UPracticeInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	UPracticeLocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer<UPracticeLocalPlayer>();
	check(LocalPlayer);
	
	if (UPracticeSettingsLocal* LocalSettings = UPracticeSettingsLocal::Get())
	{
		// Remove any registered input contexts
		const TArray<FLoadedMappableConfigPair>& Configs = LocalSettings->GetAllRegisteredInputConfigs();
		for (const FLoadedMappableConfigPair& Pair : Configs)
		{
			InputSubsystem->RemovePlayerMappableConfig(Pair.Config);
		}
		
		// Clear any player mapped keys from enhanced input
		for (const TPair<FName, FKey>& Pair : LocalSettings->GetCustomPlayerInputConfig())
		{
			InputSubsystem->RemovePlayerMappedKeyInSlot(Pair.Key);
		}
	}
}

void UPracticeInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
