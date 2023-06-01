// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeGameSettingRegistry.h"

#include "GameSettingCollection.h"
#include "PracticeSettingsLocal.h"
#include "PracticeSettingsShared.h"
#include "Player/PracticeLocalPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeGameSettingRegistry)

DEFINE_LOG_CATEGORY(LogPracticeGameSettingRegistry);

#define LOCTEXT_NAMESPACE "Practice"

//--------------------------------------
// UPracticeGameSettingRegistry
//--------------------------------------

UPracticeGameSettingRegistry::UPracticeGameSettingRegistry()
{
}

UPracticeGameSettingRegistry* UPracticeGameSettingRegistry::Get(UPracticeLocalPlayer* InLocalPlayer)
{
	UPracticeGameSettingRegistry* Registry = FindObject<UPracticeGameSettingRegistry>(InLocalPlayer, TEXT("PracticeGameSettingRegistry"), true);
	if (Registry == nullptr)
	{
		Registry = NewObject<UPracticeGameSettingRegistry>(InLocalPlayer, TEXT("PracticeGameSettingRegistry"));
		Registry->Initialize(InLocalPlayer);
	}

	return Registry;
}

bool UPracticeGameSettingRegistry::IsFinishedInitializing() const
{
	if (Super::IsFinishedInitializing())
	{
		if (UPracticeLocalPlayer* LocalPlayer = Cast<UPracticeLocalPlayer>(OwningLocalPlayer))
		{
			if (LocalPlayer->GetSharedSettings() == nullptr)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

void UPracticeGameSettingRegistry::OnInitialize(ULocalPlayer* InLocalPlayer)
{
	UPracticeLocalPlayer* PracticeLocalPlayer = Cast<UPracticeLocalPlayer>(InLocalPlayer);

	VideoSettings = InitializeVideoSettings(PracticeLocalPlayer);
	InitializeVideoSettings_FrameRates(VideoSettings, PracticeLocalPlayer);
	RegisterSetting(VideoSettings);

	AudioSettings = InitializeAudioSettings(PracticeLocalPlayer);
	RegisterSetting(AudioSettings);

	GameplaySettings = InitializeGameplaySettings(PracticeLocalPlayer);
	RegisterSetting(GameplaySettings);

	MouseAndKeyboardSettings = InitializeMouseAndKeyboardSettings(PracticeLocalPlayer);
	RegisterSetting(MouseAndKeyboardSettings);

	GamepadSettings = InitializeGamepadSettings(PracticeLocalPlayer);
	RegisterSetting(GamepadSettings);
}

void UPracticeGameSettingRegistry::SaveChanges()
{
	Super::SaveChanges();
	
	if (UPracticeLocalPlayer* LocalPlayer = Cast<UPracticeLocalPlayer>(OwningLocalPlayer))
	{
		// Game user settings need to be applied to handle things like resolution, this saves indirectly
		LocalPlayer->GetLocalSettings()->ApplySettings(false);
		
		LocalPlayer->GetSharedSettings()->ApplySettings();
		LocalPlayer->GetSharedSettings()->SaveSettings();
	}
}

#undef LOCTEXT_NAMESPACE

