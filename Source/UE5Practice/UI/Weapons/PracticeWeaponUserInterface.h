// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonUserWidget.h"

#include "PracticeWeaponUserInterface.generated.h"

class UPracticeWeaponInstance;
class UObject;
struct FGeometry;

UCLASS()
class UPracticeWeaponUserInterface : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UPracticeWeaponUserInterface(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnWeaponChanged(UPracticeWeaponInstance* OldWeapon, UPracticeWeaponInstance* NewWeapon);

private:
	void RebuildWidgetFromWeapon();

private:
	UPROPERTY(Transient)
	TObjectPtr<UPracticeWeaponInstance> CurrentInstance;
};
