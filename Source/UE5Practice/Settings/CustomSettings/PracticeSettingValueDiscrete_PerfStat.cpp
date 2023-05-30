// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeSettingValueDiscrete_PerfStat.h"

#include "CommonUIVisibilitySubsystem.h"
#include "Performance/PracticePerformanceSettings.h"
#include "Performance/PracticePerformanceStatTypes.h"
#include "Settings/PracticeSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeSettingValueDiscrete_PerfStat)

class ULocalPlayer;

#define LOCTEXT_NAMESPACE "PracticeSettings"

//////////////////////////////////////////////////////////////////////

class FGameSettingEditCondition_PerfStatAllowed : public FGameSettingEditCondition
{
public:
	static TSharedRef<FGameSettingEditCondition_PerfStatAllowed> Get(EPracticeDisplayablePerformanceStat Stat)
	{
		return MakeShared<FGameSettingEditCondition_PerfStatAllowed>(Stat);
	}

	FGameSettingEditCondition_PerfStatAllowed(EPracticeDisplayablePerformanceStat Stat)
		: AssociatedStat(Stat)
	{
	}

	//~FGameSettingEditCondition interface
	virtual void GatherEditState(const ULocalPlayer* InLocalPlayer, FGameSettingEditableState& InOutEditState) const override
	{
		const FGameplayTagContainer& VisibilityTags = UCommonUIVisibilitySubsystem::GetChecked(InLocalPlayer)->GetVisibilityTags();

		bool bCanShowStat = false;
		for (const FPracticePerformanceStatGroup& Group : GetDefault<UPracticePerformanceSettings>()->UserFacingPerformanceStats) //@TODO: Move this stuff to per-platform instead of doing vis queries too?
		{
			if (Group.AllowedStats.Contains(AssociatedStat))
			{
				const bool bShowGroup = (Group.VisibilityQuery.IsEmpty() || Group.VisibilityQuery.Matches(VisibilityTags));
				if (bShowGroup)
				{
					bCanShowStat = true;
					break;
				}
			}
		}

		if (!bCanShowStat)
		{
			InOutEditState.Hide(TEXT("Stat is not listed in UPracticePerformanceSettings or is suppressed by current platform traits"));
		}
	}
	//~End of FGameSettingEditCondition interface

private:
	EPracticeDisplayablePerformanceStat AssociatedStat;
};

//////////////////////////////////////////////////////////////////////

UPracticeSettingValueDiscrete_PerfStat::UPracticeSettingValueDiscrete_PerfStat()
{
}

void UPracticeSettingValueDiscrete_PerfStat::SetStat(EPracticeDisplayablePerformanceStat InStat)
{
	StatToDisplay = InStat;
	SetDevName(FName(*FString::Printf(TEXT("PerfStat_%d"), (int32)StatToDisplay)));
	AddEditCondition(FGameSettingEditCondition_PerfStatAllowed::Get(StatToDisplay));
}

void UPracticeSettingValueDiscrete_PerfStat::AddMode(FText&& Label, EPracticeStatDisplayMode Mode)
{
	Options.Emplace(MoveTemp(Label));
	DisplayModes.Add(Mode);
}

void UPracticeSettingValueDiscrete_PerfStat::OnInitialized()
{
	Super::OnInitialized();

	AddMode(LOCTEXT("PerfStatDisplayMode_None", "None"), EPracticeStatDisplayMode::Hidden);
	AddMode(LOCTEXT("PerfStatDisplayMode_TextOnly", "Text Only"), EPracticeStatDisplayMode::TextOnly);
	AddMode(LOCTEXT("PerfStatDisplayMode_GraphOnly", "Graph Only"), EPracticeStatDisplayMode::GraphOnly);
	AddMode(LOCTEXT("PerfStatDisplayMode_TextAndGraph", "Text and Graph"), EPracticeStatDisplayMode::TextAndGraph);
}

void UPracticeSettingValueDiscrete_PerfStat::StoreInitial()
{
	const UPracticeSettingsLocal* Settings = UPracticeSettingsLocal::Get();
	InitialMode = Settings->GetPerfStatDisplayState(StatToDisplay);
}

void UPracticeSettingValueDiscrete_PerfStat::ResetToDefault()
{
	UPracticeSettingsLocal* Settings = UPracticeSettingsLocal::Get();
	Settings->SetPerfStatDisplayState(StatToDisplay, EPracticeStatDisplayMode::Hidden);
	NotifySettingChanged(EGameSettingChangeReason::ResetToDefault);
}

void UPracticeSettingValueDiscrete_PerfStat::RestoreToInitial()
{
	UPracticeSettingsLocal* Settings = UPracticeSettingsLocal::Get();
	Settings->SetPerfStatDisplayState(StatToDisplay, InitialMode);
	NotifySettingChanged(EGameSettingChangeReason::RestoreToInitial);
}

void UPracticeSettingValueDiscrete_PerfStat::SetDiscreteOptionByIndex(int32 Index)
{
	if (DisplayModes.IsValidIndex(Index))
	{
		const EPracticeStatDisplayMode DisplayMode = DisplayModes[Index];

		UPracticeSettingsLocal* Settings = UPracticeSettingsLocal::Get();
		Settings->SetPerfStatDisplayState(StatToDisplay, DisplayMode);
	}
	NotifySettingChanged(EGameSettingChangeReason::Change);
}

int32 UPracticeSettingValueDiscrete_PerfStat::GetDiscreteOptionIndex() const
{
	const UPracticeSettingsLocal* Settings = UPracticeSettingsLocal::Get();
	return DisplayModes.Find(Settings->GetPerfStatDisplayState(StatToDisplay));
}

TArray<FText> UPracticeSettingValueDiscrete_PerfStat::GetDiscreteOptions() const
{
	return Options;
}

#undef LOCTEXT_NAMESPACE
