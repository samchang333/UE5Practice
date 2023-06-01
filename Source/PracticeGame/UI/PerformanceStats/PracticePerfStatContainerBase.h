// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonUserWidget.h"
#include "Performance/PracticePerformanceStatTypes.h"

#include "PracticePerfStatContainerBase.generated.h"

class UObject;
struct FFrame;

/**
 * UPracticePerfStatsContainerBase
 *
 * Panel that contains a set of UPracticePerfStatWidgetBase widgets and manages
 * their visibility based on user settings.
 */
 UCLASS(Abstract)
class UPracticePerfStatContainerBase : public UCommonUserWidget
{
public:
	UPracticePerfStatContainerBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	GENERATED_BODY()

	//~UUserWidget interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	//~End of UUserWidget interface

	UFUNCTION(BlueprintCallable)
	void UpdateVisibilityOfChildren();

protected:
	// Are we showing text or graph stats?
	UPROPERTY(EditAnywhere, Category=Display)
	EPracticeStatDisplayMode StatDisplayModeFilter = EPracticeStatDisplayMode::TextAndGraph;
};
