// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeReplaySubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Engine/DemoNetDriver.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeReplaySubsystem)

UPracticeReplaySubsystem::UPracticeReplaySubsystem()
{
}

void UPracticeReplaySubsystem::PlayReplay(UPracticeReplayListEntry* Replay)
{
	if (Replay != nullptr)
	{
		FString DemoName = Replay->StreamInfo.Name;
		GetGameInstance()->PlayReplay(DemoName);
	}
}

// void UPracticeReplaySubsystem::DeleteReplay()
// {
//	ReplayStreamer->DeleteFinishedStream(SelectedItem->StreamInfo.Name, FDeleteFinishedStreamCallback::CreateSP(this, &SShooterDemoList::OnDeleteFinishedStreamComplete));
// }

void UPracticeReplaySubsystem::SeekInActiveReplay(float TimeInSeconds)
{
	if (UDemoNetDriver* DemoDriver = GetDemoDriver())
	{
		DemoDriver->GotoTimeInSeconds(TimeInSeconds);
	}
}

float UPracticeReplaySubsystem::GetReplayLengthInSeconds() const
{
	if (UDemoNetDriver* DemoDriver = GetDemoDriver())
	{
		return DemoDriver->GetDemoTotalTime();
	}
	return 0.0f;
}

float UPracticeReplaySubsystem::GetReplayCurrentTime() const
{
	if (UDemoNetDriver* DemoDriver = GetDemoDriver())
	{
		return DemoDriver->GetDemoCurrentTime();
	}
	return 0.0f;
}

UDemoNetDriver* UPracticeReplaySubsystem::GetDemoDriver() const
{
	if (UWorld* World = GetGameInstance()->GetWorld())
	{
		return World->GetDemoNetDriver();
	}
	return nullptr;
}

