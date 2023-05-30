// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Info.h"
#include "System/GameplayTagStack.h"

#include "PracticeTeamInfoBase.generated.h"

namespace EEndPlayReason { enum Type : int; }

class UPracticeTeamCreationComponent;
class UPracticeTeamSubsystem;
class UObject;
struct FFrame;

UCLASS(Abstract)
class APracticeTeamInfoBase : public AInfo
{
	GENERATED_BODY()

public:
	APracticeTeamInfoBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	int32 GetTeamId() const { return TeamId; }

	//~AActor interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor interface

protected:
	virtual void RegisterWithTeamSubsystem(UPracticeTeamSubsystem* Subsystem);
	void TryRegisterWithTeamSubsystem();

private:
	void SetTeamId(int32 NewTeamId);

	UFUNCTION()
	void OnRep_TeamId();

public:
	friend UPracticeTeamCreationComponent;

	UPROPERTY(Replicated)
	FGameplayTagStackContainer TeamTags;

private:
	UPROPERTY(ReplicatedUsing=OnRep_TeamId)
	int32 TeamId;
};
