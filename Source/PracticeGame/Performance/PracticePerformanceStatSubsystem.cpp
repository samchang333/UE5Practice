// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticePerformanceStatSubsystem.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/NetConnection.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "GameModes/PracticeGameState.h"
#include "Performance/PracticePerformanceStatTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticePerformanceStatSubsystem)

class FSubsystemCollectionBase;

//////////////////////////////////////////////////////////////////////
// FPracticePerformanceStatCache

void FPracticePerformanceStatCache::StartCharting()
{
}

void FPracticePerformanceStatCache::ProcessFrame(const FFrameData& FrameData)
{
	CachedData = FrameData;
	CachedServerFPS = 0.0f;
	CachedPingMS = 0.0f;
	CachedPacketLossIncomingPercent = 0.0f;
	CachedPacketLossOutgoingPercent = 0.0f;
	CachedPacketRateIncoming = 0.0f;
	CachedPacketRateOutgoing = 0.0f;
	CachedPacketSizeIncoming = 0.0f;
	CachedPacketSizeOutgoing = 0.0f;

	if (UWorld* World = MySubsystem->GetGameInstance()->GetWorld())
	{
		if (const APracticeGameState* GameState = World->GetGameState<APracticeGameState>())
		{
			CachedServerFPS = GameState->GetServerFPS();
		}

		if (APlayerController* LocalPC = GEngine->GetFirstLocalPlayerController(World))
		{
			if (APlayerState* PS = LocalPC->GetPlayerState<APlayerState>())
			{
				CachedPingMS = PS->GetPingInMilliseconds();
			}

			if (UNetConnection* NetConnection = LocalPC->GetNetConnection())
			{
				const UNetConnection::FNetConnectionPacketLoss& InLoss = NetConnection->GetInLossPercentage();
				CachedPacketLossIncomingPercent = InLoss.GetAvgLossPercentage();
				const UNetConnection::FNetConnectionPacketLoss& OutLoss = NetConnection->GetOutLossPercentage();
				CachedPacketLossOutgoingPercent = OutLoss.GetAvgLossPercentage();

				CachedPacketRateIncoming = NetConnection->InPacketsPerSecond;
				CachedPacketRateOutgoing = NetConnection->OutPacketsPerSecond;

				CachedPacketSizeIncoming = (NetConnection->InPacketsPerSecond != 0) ? (NetConnection->InBytesPerSecond / (float)NetConnection->InPacketsPerSecond) : 0.0f;
				CachedPacketSizeOutgoing = (NetConnection->OutPacketsPerSecond != 0) ? (NetConnection->OutBytesPerSecond / (float)NetConnection->OutPacketsPerSecond) : 0.0f;
			}
		}
	}
}

void FPracticePerformanceStatCache::StopCharting()
{
}

double FPracticePerformanceStatCache::GetCachedStat(EPracticeDisplayablePerformanceStat Stat) const
{
	static_assert((int32)EPracticeDisplayablePerformanceStat::Count == 15, "Need to update this function to deal with new performance stats");
	switch (Stat)
	{
	case EPracticeDisplayablePerformanceStat::ClientFPS:
		return (CachedData.TrueDeltaSeconds != 0.0) ? (1.0 / CachedData.TrueDeltaSeconds) : 0.0;
	case EPracticeDisplayablePerformanceStat::ServerFPS:
		return CachedServerFPS;
	case EPracticeDisplayablePerformanceStat::IdleTime:
		return CachedData.IdleSeconds;
	case EPracticeDisplayablePerformanceStat::FrameTime:
		return CachedData.TrueDeltaSeconds;
	case EPracticeDisplayablePerformanceStat::FrameTime_GameThread:
		return CachedData.GameThreadTimeSeconds;
	case EPracticeDisplayablePerformanceStat::FrameTime_RenderThread:
		return CachedData.RenderThreadTimeSeconds;
	case EPracticeDisplayablePerformanceStat::FrameTime_RHIThread:
		return CachedData.RHIThreadTimeSeconds;
	case EPracticeDisplayablePerformanceStat::FrameTime_GPU:
		return CachedData.GPUTimeSeconds;
	case EPracticeDisplayablePerformanceStat::Ping:
		return CachedPingMS;
	case EPracticeDisplayablePerformanceStat::PacketLoss_Incoming:
		return CachedPacketLossIncomingPercent;
	case EPracticeDisplayablePerformanceStat::PacketLoss_Outgoing:
		return CachedPacketLossOutgoingPercent;
	case EPracticeDisplayablePerformanceStat::PacketRate_Incoming:
		return CachedPacketRateIncoming;
	case EPracticeDisplayablePerformanceStat::PacketRate_Outgoing:
		return CachedPacketRateOutgoing;
	case EPracticeDisplayablePerformanceStat::PacketSize_Incoming:
		return CachedPacketSizeIncoming;
	case EPracticeDisplayablePerformanceStat::PacketSize_Outgoing:
		return CachedPacketSizeOutgoing;
	}

	return 0.0f;
}

//////////////////////////////////////////////////////////////////////
// UPracticePerformanceStatSubsystem

void UPracticePerformanceStatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Tracker = MakeShared<FPracticePerformanceStatCache>(this);
	GEngine->AddPerformanceDataConsumer(Tracker);
}

void UPracticePerformanceStatSubsystem::Deinitialize()
{
	GEngine->RemovePerformanceDataConsumer(Tracker);
	Tracker.Reset();
}

double UPracticePerformanceStatSubsystem::GetCachedStat(EPracticeDisplayablePerformanceStat Stat) const
{
	return Tracker->GetCachedStat(Stat);
}

