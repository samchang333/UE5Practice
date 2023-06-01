// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonActivatableWidget.h"

#include "PracticeControllerDisconnectedScreen.generated.h"

class UHorizontalBox;
class UObject;

UCLASS(Abstract, BlueprintType, Blueprintable)
class UPracticeControllerDisconnectedScreen : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	void NativeOnActivated() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> HBox_SwitchUser;
};
