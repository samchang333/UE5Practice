// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "DataSource/GameSettingDataSourceDynamic.h" // IWYU pragma: keep
#include "GameSettingRegistry.h"
#include "Settings/PracticeSettingsLocal.h" // IWYU pragma: keep

#include "PracticeGameSettingRegistry.generated.h"

class ULocalPlayer;
class UObject;

//--------------------------------------
// UPracticeGameSettingRegistry
//--------------------------------------

class UGameSettingCollection;
class UPracticeLocalPlayer;

DECLARE_LOG_CATEGORY_EXTERN(LogPracticeGameSettingRegistry, Log, Log);

#define GET_SHARED_SETTINGS_FUNCTION_PATH(FunctionOrPropertyName)							\
	MakeShared<FGameSettingDataSourceDynamic>(TArray<FString>({								\
		GET_FUNCTION_NAME_STRING_CHECKED(UPracticeLocalPlayer, GetSharedSettings),				\
		GET_FUNCTION_NAME_STRING_CHECKED(UPracticeSettingsShared, FunctionOrPropertyName)		\
	}))

#define GET_LOCAL_SETTINGS_FUNCTION_PATH(FunctionOrPropertyName)							\
	MakeShared<FGameSettingDataSourceDynamic>(TArray<FString>({								\
		GET_FUNCTION_NAME_STRING_CHECKED(UPracticeLocalPlayer, GetLocalSettings),				\
		GET_FUNCTION_NAME_STRING_CHECKED(UPracticeSettingsLocal, FunctionOrPropertyName)		\
	}))

/**
 * 
 */
UCLASS()
class UPracticeGameSettingRegistry : public UGameSettingRegistry
{
	GENERATED_BODY()

public:
	UPracticeGameSettingRegistry();

	static UPracticeGameSettingRegistry* Get(UPracticeLocalPlayer* InLocalPlayer);
	
	virtual void SaveChanges() override;

protected:
	virtual void OnInitialize(ULocalPlayer* InLocalPlayer) override;
	virtual bool IsFinishedInitializing() const override;

	UGameSettingCollection* InitializeVideoSettings(UPracticeLocalPlayer* InLocalPlayer);
	void InitializeVideoSettings_FrameRates(UGameSettingCollection* Screen, UPracticeLocalPlayer* InLocalPlayer);
	void AddPerformanceStatPage(UGameSettingCollection* Screen, UPracticeLocalPlayer* InLocalPlayer);

	UGameSettingCollection* InitializeAudioSettings(UPracticeLocalPlayer* InLocalPlayer);
	UGameSettingCollection* InitializeGameplaySettings(UPracticeLocalPlayer* InLocalPlayer);

	UGameSettingCollection* InitializeMouseAndKeyboardSettings(UPracticeLocalPlayer* InLocalPlayer);
	UGameSettingCollection* InitializeGamepadSettings(UPracticeLocalPlayer* InLocalPlayer);

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> VideoSettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> AudioSettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> GameplaySettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> MouseAndKeyboardSettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> GamepadSettings;
};
