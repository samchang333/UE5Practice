// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeTeamPublicInfo.h"

#include "Net/UnrealNetwork.h"
#include "Teams/PracticeTeamInfoBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeTeamPublicInfo)

class FLifetimeProperty;

APracticeTeamPublicInfo::APracticeTeamPublicInfo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void APracticeTeamPublicInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, TeamDisplayAsset, COND_InitialOnly);
}

void APracticeTeamPublicInfo::SetTeamDisplayAsset(TObjectPtr<UPracticeTeamDisplayAsset> NewDisplayAsset)
{
	check(HasAuthority());
	check(TeamDisplayAsset == nullptr);

	TeamDisplayAsset = NewDisplayAsset;

	TryRegisterWithTeamSubsystem();
}

void APracticeTeamPublicInfo::OnRep_TeamDisplayAsset()
{
	TryRegisterWithTeamSubsystem();
}

