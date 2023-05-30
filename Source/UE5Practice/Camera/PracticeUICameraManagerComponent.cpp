// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeUICameraManagerComponent.h"

#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "PracticePlayerCameraManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeUICameraManagerComponent)

class AActor;
class FDebugDisplayInfo;

UPracticeUICameraManagerComponent* UPracticeUICameraManagerComponent::GetComponent(APlayerController* PC)
{
	if (PC != nullptr)
	{
		if (APracticePlayerCameraManager* PCCamera = Cast<APracticePlayerCameraManager>(PC->PlayerCameraManager))
		{
			return PCCamera->GetUICameraComponent();
		}
	}

	return nullptr;
}

UPracticeUICameraManagerComponent::UPracticeUICameraManagerComponent()
{
	bWantsInitializeComponent = true;

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		// Register "showdebug" hook.
		if (!IsRunningDedicatedServer())
		{
			AHUD::OnShowDebugInfo.AddUObject(this, &ThisClass::OnShowDebugInfo);
		}
	}
}

void UPracticeUICameraManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UPracticeUICameraManagerComponent::SetViewTarget(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams)
{
	TGuardValue<bool> UpdatingViewTargetGuard(bUpdatingViewTarget, true);

	ViewTarget = InViewTarget;
	CastChecked<APracticePlayerCameraManager>(GetOwner())->SetViewTarget(ViewTarget, TransitionParams);
}

bool UPracticeUICameraManagerComponent::NeedsToUpdateViewTarget() const
{
	return false;
}

void UPracticeUICameraManagerComponent::UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime)
{
}

void UPracticeUICameraManagerComponent::OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos)
{
}
