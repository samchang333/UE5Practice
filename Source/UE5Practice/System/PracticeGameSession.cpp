// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeGameSession.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeGameSession)


APracticeGameSession::APracticeGameSession(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool APracticeGameSession::ProcessAutoLogin()
{
	// This is actually handled in PracticeGameMode::TryDedicatedServerLogin
	return true;
}

void APracticeGameSession::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
}

void APracticeGameSession::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
}

