// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PracticeActivatableWidget.h"

#include "PracticeHUDLayout.generated.h"

class UCommonActivatableWidget;
class UObject;


/**
 * UPracticeHUDLayout
 *
 *	Widget used to lay out the player's HUD (typically specified by an Add Widgets action in the experience)
 */
UCLASS(Abstract, BlueprintType, Blueprintable, Meta = (DisplayName = "Practice HUD Layout", Category = "Practice|HUD"))
class UPracticeHUDLayout : public UPracticeActivatableWidget
{
	GENERATED_BODY()

public:

	UPracticeHUDLayout(const FObjectInitializer& ObjectInitializer);

	void NativeOnInitialized() override;

protected:
	void HandleEscapeAction();

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UCommonActivatableWidget> EscapeMenuClass;
};
