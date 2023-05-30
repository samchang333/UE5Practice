// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeDeveloperSettings.h"
#include "Misc/App.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeDeveloperSettings)

#define LOCTEXT_NAMESPACE "PracticeCheats"

UPracticeDeveloperSettings::UPracticeDeveloperSettings()
{
}

FName UPracticeDeveloperSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}

#if WITH_EDITOR
void UPracticeDeveloperSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	ApplySettings();
}

void UPracticeDeveloperSettings::PostReloadConfig(FProperty* PropertyThatWasLoaded)
{
	Super::PostReloadConfig(PropertyThatWasLoaded);

	ApplySettings();
}

void UPracticeDeveloperSettings::PostInitProperties()
{
	Super::PostInitProperties();

	ApplySettings();
}

void UPracticeDeveloperSettings::ApplySettings()
{
}

void UPracticeDeveloperSettings::OnPlayInEditorStarted() const
{
	// Show a notification toast to remind the user that there's an experience override set
	if (ExperienceOverride.IsValid())
	{
		FNotificationInfo Info(FText::Format(
			LOCTEXT("ExperienceOverrideActive", "Developer Settings Override\nExperience {0}"),
			FText::FromName(ExperienceOverride.PrimaryAssetName)
		));
		Info.ExpireDuration = 2.0f;
		FSlateNotificationManager::Get().AddNotification(Info);
	}
}
#endif

#undef LOCTEXT_NAMESPACE

