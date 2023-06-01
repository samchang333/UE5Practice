// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeSettingValueDiscrete_MobileFPSType.h"

#include "Performance/PracticePerformanceSettings.h"
#include "Settings/PracticeSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeSettingValueDiscrete_MobileFPSType)

#define LOCTEXT_NAMESPACE "PracticeSettings"

UPracticeSettingValueDiscrete_MobileFPSType::UPracticeSettingValueDiscrete_MobileFPSType()
{
}

void UPracticeSettingValueDiscrete_MobileFPSType::OnInitialized()
{
	Super::OnInitialized();

	const UPracticePlatformSpecificRenderingSettings* PlatformSettings = UPracticePlatformSpecificRenderingSettings::Get();
	const UPracticeSettingsLocal* UserSettings = UPracticeSettingsLocal::Get();

	for (int32 TestLimit : PlatformSettings->MobileFrameRateLimits)
	{
		if (UPracticeSettingsLocal::IsSupportedMobileFramePace(TestLimit))
		{
			FPSOptions.Add(TestLimit, MakeLimitString(TestLimit));
		}
	}

	const int32 FirstFrameRateWithQualityLimit = UserSettings->GetFirstFrameRateWithQualityLimit();
	if (FirstFrameRateWithQualityLimit > 0)
	{
		SetWarningRichText(FText::Format(LOCTEXT("MobileFPSType_Note", "<strong>Note: Changing the framerate setting to {0} or higher might lower your Quality Presets.</>"), MakeLimitString(FirstFrameRateWithQualityLimit)));
	}
}

int32 UPracticeSettingValueDiscrete_MobileFPSType::GetDefaultFPS() const
{
	return UPracticeSettingsLocal::GetDefaultMobileFrameRate();
}

FText UPracticeSettingValueDiscrete_MobileFPSType::MakeLimitString(int32 Number)
{
	return FText::Format(LOCTEXT("MobileFrameRateOption", "{0} FPS"), FText::AsNumber(Number));
}

void UPracticeSettingValueDiscrete_MobileFPSType::StoreInitial()
{
	InitialValue = GetValue();
}

void UPracticeSettingValueDiscrete_MobileFPSType::ResetToDefault()
{
	SetValue(GetDefaultFPS(), EGameSettingChangeReason::ResetToDefault);
}

void UPracticeSettingValueDiscrete_MobileFPSType::RestoreToInitial()
{
	SetValue(InitialValue, EGameSettingChangeReason::RestoreToInitial);
}

void UPracticeSettingValueDiscrete_MobileFPSType::SetDiscreteOptionByIndex(int32 Index)
{
	TArray<int32> FPSOptionsModes;
	FPSOptions.GenerateKeyArray(FPSOptionsModes);

	int32 NewMode = FPSOptionsModes.IsValidIndex(Index) ? FPSOptionsModes[Index] : GetDefaultFPS();

	SetValue(NewMode, EGameSettingChangeReason::Change);
}

int32 UPracticeSettingValueDiscrete_MobileFPSType::GetDiscreteOptionIndex() const
{
	TArray<int32> FPSOptionsModes;
	FPSOptions.GenerateKeyArray(FPSOptionsModes);
	return FPSOptionsModes.IndexOfByKey(GetValue());
}

TArray<FText> UPracticeSettingValueDiscrete_MobileFPSType::GetDiscreteOptions() const
{
	TArray<FText> Options;
	FPSOptions.GenerateValueArray(Options);

	return Options;
}

int32 UPracticeSettingValueDiscrete_MobileFPSType::GetValue() const
{
	return UPracticeSettingsLocal::Get()->GetDesiredMobileFrameRateLimit();
}

void UPracticeSettingValueDiscrete_MobileFPSType::SetValue(int32 NewLimitFPS, EGameSettingChangeReason InReason)
{
	UPracticeSettingsLocal::Get()->SetDesiredMobileFrameRateLimit(NewLimitFPS);

	NotifySettingChanged(InReason);
}

#undef LOCTEXT_NAMESPACE

