// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeSettingValueDiscrete_Language.h"

#include "Internationalization/Culture.h"
#include "Internationalization/TextLocalizationManager.h"
#include "Messaging/CommonGameDialog.h"
#include "Messaging/CommonMessagingSubsystem.h"
#include "Player/PracticeLocalPlayer.h"
#include "Settings/PracticeSettingsShared.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeSettingValueDiscrete_Language)

#define LOCTEXT_NAMESPACE "Practice"

static const int32 SettingSystemDefaultLanguageIndex = 0;

UPracticeSettingValueDiscrete_Language::UPracticeSettingValueDiscrete_Language()
{
}

void UPracticeSettingValueDiscrete_Language::OnInitialized()
{
	Super::OnInitialized();

	const TArray<FString> AllCultureNames = FTextLocalizationManager::Get().GetLocalizedCultureNames(ELocalizationLoadFlags::Game);
	for (const FString& CultureName : AllCultureNames)
	{
		if (FInternationalization::Get().IsCultureAllowed(CultureName))
		{
			AvailableCultureNames.Add(CultureName);
		}
	}

	AvailableCultureNames.Insert(TEXT(""), SettingSystemDefaultLanguageIndex);
}

void UPracticeSettingValueDiscrete_Language::StoreInitial()
{
	// ?
}

void UPracticeSettingValueDiscrete_Language::OnApply()
{
	if (UCommonMessagingSubsystem* Messaging = LocalPlayer->GetSubsystem<UCommonMessagingSubsystem>())
	{
		Messaging->ShowConfirmation(
			UCommonGameDialogDescriptor::CreateConfirmationOk(
			LOCTEXT("WarningLanguage_Title", "Language Changed"),
			LOCTEXT("WarningLanguage_Message", "You will need to restart the game completely for all language related changes to take effect.")
			)
		);
	}
}

void UPracticeSettingValueDiscrete_Language::ResetToDefault()
{
	SetDiscreteOptionByIndex(SettingSystemDefaultLanguageIndex);
}

void UPracticeSettingValueDiscrete_Language::RestoreToInitial()
{
	if (UPracticeSettingsShared* Settings = CastChecked<UPracticeLocalPlayer>(LocalPlayer)->GetSharedSettings())
	{
		Settings->ClearPendingCulture();
		NotifySettingChanged(EGameSettingChangeReason::RestoreToInitial);
	}
}

void UPracticeSettingValueDiscrete_Language::SetDiscreteOptionByIndex(int32 Index)
{
	if (UPracticeSettingsShared* Settings = CastChecked<UPracticeLocalPlayer>(LocalPlayer)->GetSharedSettings())
	{
		if (Index == SettingSystemDefaultLanguageIndex)
		{
			Settings->ResetToDefaultCulture();
		}
		else if (AvailableCultureNames.IsValidIndex(Index))
		{
			Settings->SetPendingCulture(AvailableCultureNames[Index]);
		}
		
		NotifySettingChanged(EGameSettingChangeReason::Change);
	}
}

int32 UPracticeSettingValueDiscrete_Language::GetDiscreteOptionIndex() const
{
	if (const UPracticeSettingsShared* Settings = CastChecked<UPracticeLocalPlayer>(LocalPlayer)->GetSharedSettings())
	{
		if (Settings->ShouldResetToDefaultCulture())
		{
			return SettingSystemDefaultLanguageIndex;
		}

		// We prefer the pending culture to the current culture as the options UI updates the pending culture before it 
		// gets applied, and we need the UI to reflect that choice
		FString PendingCulture = Settings->GetPendingCulture();
		if (PendingCulture.IsEmpty())
		{
			if (Settings->IsUsingDefaultCulture())
			{
				return SettingSystemDefaultLanguageIndex;
			}

			PendingCulture = FInternationalization::Get().GetCurrentCulture()->GetName();
		}

		// Try to find an exact match 
		{
			const int32 ExactMatchIndex = AvailableCultureNames.IndexOfByKey(PendingCulture);
			if (ExactMatchIndex != INDEX_NONE)
			{
				return ExactMatchIndex;
			}
		}

		// Try to find a prioritized match (eg, allowing "en-US" to show as "en" in the UI)
		const TArray<FString> PrioritizedPendingCultures = FInternationalization::Get().GetPrioritizedCultureNames(PendingCulture);
		for (int32 i = 0; i < AvailableCultureNames.Num(); ++i)
		{
			if (PrioritizedPendingCultures.Contains(AvailableCultureNames[i]))
			{
				return i;
			}
		}
	}

	return 0;
}

TArray<FText> UPracticeSettingValueDiscrete_Language::GetDiscreteOptions() const
{
	TArray<FText> Options;

	for (const FString& CultureName : AvailableCultureNames)
	{
		if (CultureName == TEXT(""))
		{
			const FCulturePtr SystemDefaultCulture = FInternationalization::Get().GetDefaultCulture();
			if (ensure(SystemDefaultCulture))
			{
				const FString& DefaultCultureDisplayName = SystemDefaultCulture->GetDisplayName();
				FText LocalizedSystemDefault = FText::Format(LOCTEXT("SystemDefaultLanguage", "System Default ({0})"), FText::FromString(DefaultCultureDisplayName));

				Options.Add(MoveTemp(LocalizedSystemDefault));
			}
		}
		else
		{
			FCulturePtr Culture = FInternationalization::Get().GetCulture(CultureName);
			if (ensureMsgf(Culture, TEXT("Unable to find Culture '%s'!"), *CultureName))
			{
				const FString CultureDisplayName = Culture->GetDisplayName();
				const FString CultureNativeName = Culture->GetNativeName();

				// Only show both names if they're different (to avoid repetition)
				FString Entry = (!CultureNativeName.Equals(CultureDisplayName, ESearchCase::CaseSensitive))
					? FString::Printf(TEXT("%s (%s)"), *CultureNativeName, *CultureDisplayName)
					: CultureNativeName;

				Options.Add(FText::FromString(Entry));
			}
		}
	}

	return Options;
}

#undef LOCTEXT_NAMESPACE
