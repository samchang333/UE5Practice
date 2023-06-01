// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonUserWidget.h"

#include "PracticePerfStatWidgetBase.generated.h"

enum class EPracticeDisplayablePerformanceStat : uint8;

class UPracticePerformanceStatSubsystem;
class UObject;
struct FFrame;

/**
 * UPracticePerfStatWidgetBase
 *
 * Base class for a widget that displays a single stat, e.g., FPS, ping, etc...
 */
 UCLASS(Abstract)
class UPracticePerfStatWidgetBase : public UCommonUserWidget
{
public:
	UPracticePerfStatWidgetBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	GENERATED_BODY()

public:
	// Returns the stat this widget is supposed to display
	UFUNCTION(BlueprintPure)
	EPracticeDisplayablePerformanceStat GetStatToDisplay() const
	{
		return StatToDisplay;
	}

	// Polls for the value of this stat (unscaled)
	UFUNCTION(BlueprintPure)
	double FetchStatValue();

protected:
	// Cached subsystem pointer
	UPROPERTY(Transient)
	TObjectPtr<UPracticePerformanceStatSubsystem> CachedStatSubsystem;

	// The stat to display
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Display)
	EPracticeDisplayablePerformanceStat StatToDisplay;
 };
