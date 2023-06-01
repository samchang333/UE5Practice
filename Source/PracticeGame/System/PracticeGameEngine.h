// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/GameEngine.h"

#include "PracticeGameEngine.generated.h"

class IEngineLoop;
class UObject;


UCLASS()
class UPracticeGameEngine : public UGameEngine
{
	GENERATED_BODY()

public:

	UPracticeGameEngine(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void Init(IEngineLoop* InEngineLoop) override;
};
