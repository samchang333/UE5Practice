// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticePlayerCameraManager.h"

#include "Async/TaskGraphInterfaces.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "PracticeCameraComponent.h"
#include "PracticeUICameraManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticePlayerCameraManager)

class FDebugDisplayInfo;

static FName UICameraComponentName(TEXT("UICamera"));

APracticePlayerCameraManager::APracticePlayerCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultFOV = PRACTICE_CAMERA_DEFAULT_FOV;
	ViewPitchMin = PRACTICE_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = PRACTICE_CAMERA_DEFAULT_PITCH_MAX;

	UICamera = CreateDefaultSubobject<UPracticeUICameraManagerComponent>(UICameraComponentName);
}

UPracticeUICameraManagerComponent* APracticePlayerCameraManager::GetUICameraComponent() const
{
	return UICamera;
}

void APracticePlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	// If the UI Camera is looking at something, let it have priority.
	if (UICamera->NeedsToUpdateViewTarget())
	{
		Super::UpdateViewTarget(OutVT, DeltaTime);
		UICamera->UpdateViewTarget(OutVT, DeltaTime);
		return;
	}

	Super::UpdateViewTarget(OutVT, DeltaTime);
}

void APracticePlayerCameraManager::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	check(Canvas);

	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	DisplayDebugManager.SetFont(GEngine->GetSmallFont());
	DisplayDebugManager.SetDrawColor(FColor::Yellow);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("PracticePlayerCameraManager: %s"), *GetNameSafe(this)));

	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	const APawn* Pawn = (PCOwner ? PCOwner->GetPawn() : nullptr);

	if (const UPracticeCameraComponent* CameraComponent = UPracticeCameraComponent::FindCameraComponent(Pawn))
	{
		CameraComponent->DrawDebug(Canvas);
	}
}

