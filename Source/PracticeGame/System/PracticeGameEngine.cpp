// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeGameEngine.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeGameEngine)

class IEngineLoop;


UPracticeGameEngine::UPracticeGameEngine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPracticeGameEngine::Init(IEngineLoop* InEngineLoop)
{
	Super::Init(InEngineLoop);
}

