// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PracticeTeamInfoBase.h"

#include "PracticeTeamPublicInfo.generated.h"

class UPracticeTeamCreationComponent;
class UPracticeTeamDisplayAsset;
class UObject;
struct FFrame;

UCLASS()
class APracticeTeamPublicInfo : public APracticeTeamInfoBase
{
	GENERATED_BODY()

	friend UPracticeTeamCreationComponent;

public:
	APracticeTeamPublicInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPracticeTeamDisplayAsset* GetTeamDisplayAsset() const { return TeamDisplayAsset; }

private:
	UFUNCTION()
	void OnRep_TeamDisplayAsset();

	void SetTeamDisplayAsset(TObjectPtr<UPracticeTeamDisplayAsset> NewDisplayAsset);

private:
	UPROPERTY(ReplicatedUsing=OnRep_TeamDisplayAsset)
	TObjectPtr<UPracticeTeamDisplayAsset> TeamDisplayAsset;
};
