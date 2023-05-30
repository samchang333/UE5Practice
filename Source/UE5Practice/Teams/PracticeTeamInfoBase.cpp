// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeTeamInfoBase.h"

#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Teams/PracticeTeamSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeTeamInfoBase)

class FLifetimeProperty;

APracticeTeamInfoBase::APracticeTeamInfoBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TeamId(INDEX_NONE)
{
	bReplicates = true;
	bAlwaysRelevant = true;
	NetPriority = 3.0f;
	SetReplicatingMovement(false);
}

void APracticeTeamInfoBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TeamTags);
	DOREPLIFETIME_CONDITION(ThisClass, TeamId, COND_InitialOnly);
}

void APracticeTeamInfoBase::BeginPlay()
{
	Super::BeginPlay();

	TryRegisterWithTeamSubsystem();
}

void APracticeTeamInfoBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TeamId != INDEX_NONE)
	{
		UPracticeTeamSubsystem* TeamSubsystem = GetWorld()->GetSubsystem<UPracticeTeamSubsystem>();
		if (TeamSubsystem)
		{
			// EndPlay can happen at weird times where the subsystem has already been destroyed
			TeamSubsystem->UnregisterTeamInfo(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void APracticeTeamInfoBase::RegisterWithTeamSubsystem(UPracticeTeamSubsystem* Subsystem)
{
	Subsystem->RegisterTeamInfo(this);
}

void APracticeTeamInfoBase::TryRegisterWithTeamSubsystem()
{
	if (TeamId != INDEX_NONE)
	{
		UPracticeTeamSubsystem* TeamSubsystem = GetWorld()->GetSubsystem<UPracticeTeamSubsystem>();
		if (ensure(TeamSubsystem))
		{
			RegisterWithTeamSubsystem(TeamSubsystem);
		}
	}
}

void APracticeTeamInfoBase::SetTeamId(int32 NewTeamId)
{
	check(HasAuthority());
	check(TeamId == INDEX_NONE);
	check(NewTeamId != INDEX_NONE);

	TeamId = NewTeamId;

	TryRegisterWithTeamSubsystem();
}

void APracticeTeamInfoBase::OnRep_TeamId()
{
	TryRegisterWithTeamSubsystem();
}

