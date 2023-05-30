// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticePerfStatContainerBase.h"

#include "Blueprint/WidgetTree.h"
#include "PracticePerfStatWidgetBase.h"
#include "Settings/PracticeSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticePerfStatContainerBase)

//////////////////////////////////////////////////////////////////////
// UPracticePerfStatsContainerBase

UPracticePerfStatContainerBase::UPracticePerfStatContainerBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPracticePerfStatContainerBase::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateVisibilityOfChildren();

	UPracticeSettingsLocal::Get()->OnPerfStatDisplayStateChanged().AddUObject(this, &ThisClass::UpdateVisibilityOfChildren);
}

void UPracticePerfStatContainerBase::NativeDestruct()
{
	UPracticeSettingsLocal::Get()->OnPerfStatDisplayStateChanged().RemoveAll(this);

	Super::NativeDestruct();
}

void UPracticePerfStatContainerBase::UpdateVisibilityOfChildren()
{
	UPracticeSettingsLocal* UserSettings = UPracticeSettingsLocal::Get();

	const bool bShowTextWidgets = (StatDisplayModeFilter == EPracticeStatDisplayMode::TextOnly) || (StatDisplayModeFilter == EPracticeStatDisplayMode::TextAndGraph);
	const bool bShowGraphWidgets = (StatDisplayModeFilter == EPracticeStatDisplayMode::GraphOnly) || (StatDisplayModeFilter == EPracticeStatDisplayMode::TextAndGraph);
	
	check(WidgetTree);
	WidgetTree->ForEachWidget([&](UWidget* Widget)
	{
		if (UPracticePerfStatWidgetBase* TypedWidget = Cast<UPracticePerfStatWidgetBase>(Widget))
		{
			const EPracticeStatDisplayMode SettingMode = UserSettings->GetPerfStatDisplayState(TypedWidget->GetStatToDisplay());

			bool bShowWidget = false;
			switch (SettingMode)
			{
			case EPracticeStatDisplayMode::Hidden:
				bShowWidget = false;
				break;
			case EPracticeStatDisplayMode::TextOnly:
				bShowWidget = bShowTextWidgets;
				break;
			case EPracticeStatDisplayMode::GraphOnly:
				bShowWidget = bShowGraphWidgets;
				break;
			case EPracticeStatDisplayMode::TextAndGraph:
				bShowWidget = bShowTextWidgets || bShowGraphWidgets;
				break;
			}

			TypedWidget->SetVisibility(bShowWidget ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		}
	});
}

