// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeDebugCameraController.h"
#include "PracticeCheatManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeDebugCameraController)


APracticeDebugCameraController::APracticeDebugCameraController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Use the same cheat class as PracticePlayerController to allow toggling the debug camera through cheats.
	CheatClass = UPracticeCheatManager::StaticClass();
}

void APracticeDebugCameraController::AddCheats(bool bForce)
{
	// Mirrors PracticePlayerController's AddCheats() to avoid the player becoming stuck in the debug camera.
#if USING_CHEAT_MANAGER
	Super::AddCheats(true);
#else //#if USING_CHEAT_MANAGER
	Super::AddCheats(bForce);
#endif // #else //#if USING_CHEAT_MANAGER
}

