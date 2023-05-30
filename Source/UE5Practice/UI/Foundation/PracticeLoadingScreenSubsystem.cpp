// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Foundation/PracticeLoadingScreenSubsystem.h"

#include "Blueprint/UserWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeLoadingScreenSubsystem)

class UUserWidget;

//////////////////////////////////////////////////////////////////////
// UPracticeLoadingScreenSubsystem

UPracticeLoadingScreenSubsystem::UPracticeLoadingScreenSubsystem()
{
}

void UPracticeLoadingScreenSubsystem::SetLoadingScreenContentWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (LoadingScreenWidgetClass != NewWidgetClass)
	{
		LoadingScreenWidgetClass = NewWidgetClass;

		OnLoadingScreenWidgetChanged.Broadcast(LoadingScreenWidgetClass);
	}
}

TSubclassOf<UUserWidget> UPracticeLoadingScreenSubsystem::GetLoadingScreenContentWidget() const
{
	return LoadingScreenWidgetClass;
}

