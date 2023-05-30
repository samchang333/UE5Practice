// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DebugCameraController.h"

#include "PracticeDebugCameraController.generated.h"

class UObject;


/**
 * APracticeDebugCameraController
 *
 *	Used for controlling the debug camera when it is enabled via the cheat manager.
 */
UCLASS()
class APracticeDebugCameraController : public ADebugCameraController
{
	GENERATED_BODY()

public:

	APracticeDebugCameraController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void AddCheats(bool bForce) override;
};
