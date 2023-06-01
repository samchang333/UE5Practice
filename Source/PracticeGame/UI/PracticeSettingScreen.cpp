// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeSettingScreen.h"

#include "Input/CommonUIInputTypes.h"
#include "Player/PracticeLocalPlayer.h"
#include "Settings/PracticeGameSettingRegistry.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeSettingScreen)

class UGameSettingRegistry;

void UPracticeSettingScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BackHandle = RegisterUIActionBinding(FBindUIActionArgs(BackInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleBackAction)));
	ApplyHandle = RegisterUIActionBinding(FBindUIActionArgs(ApplyInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleApplyAction)));
	CancelChangesHandle = RegisterUIActionBinding(FBindUIActionArgs(CancelChangesInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleCancelChangesAction)));
}

UGameSettingRegistry* UPracticeSettingScreen::CreateRegistry()
{
	UPracticeGameSettingRegistry* NewRegistry = NewObject<UPracticeGameSettingRegistry>();

	if (UPracticeLocalPlayer* LocalPlayer = CastChecked<UPracticeLocalPlayer>(GetOwningLocalPlayer()))
	{
		NewRegistry->Initialize(LocalPlayer);
	}

	return NewRegistry;
}

void UPracticeSettingScreen::HandleBackAction()
{
	if (AttemptToPopNavigation())
	{
		return;
	}

	ApplyChanges();

	DeactivateWidget();
}

void UPracticeSettingScreen::HandleApplyAction()
{
	ApplyChanges();
}

void UPracticeSettingScreen::HandleCancelChangesAction()
{
	CancelChanges();
}

void UPracticeSettingScreen::OnSettingsDirtyStateChanged_Implementation(bool bSettingsDirty)
{
	if (bSettingsDirty)
	{
		if (!GetActionBindings().Contains(ApplyHandle))
		{
			AddActionBinding(ApplyHandle);
		}
		if (!GetActionBindings().Contains(CancelChangesHandle))
		{
			AddActionBinding(CancelChangesHandle);
		}
	}
	else
	{
		RemoveActionBinding(ApplyHandle);
		RemoveActionBinding(CancelChangesHandle);
	}
}
