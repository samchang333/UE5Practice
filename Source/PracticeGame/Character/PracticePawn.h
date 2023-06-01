// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModularPawn.h"
#include "Teams/PracticeTeamAgentInterface.h"

#include "PracticePawn.generated.h"

class AController;
class UObject;
struct FFrame;

/**
 * APracticePawn
 */
UCLASS()
class PRACTICEGAME_API APracticePawn : public AModularPawn, public IPracticeTeamAgentInterface
{
	GENERATED_BODY()

public:

	APracticePawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor interface

	//~APawn interface
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	//~End of APawn interface

	//~IPracticeTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FOnPracticeTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of IPracticeTeamAgentInterface interface

protected:
	// Called to determine what happens to the team ID when possession ends
	virtual FGenericTeamId DetermineNewTeamAfterPossessionEnds(FGenericTeamId OldTeamID) const
	{
		// This could be changed to return, e.g., OldTeamID if you want to keep it assigned afterwards, or return an ID for some neutral faction, or etc...
		return FGenericTeamId::NoTeam;
	}

private:
	UFUNCTION()
	void OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

private:
	UPROPERTY(ReplicatedUsing = OnRep_MyTeamID)
	FGenericTeamId MyTeamID;

	UPROPERTY()
	FOnPracticeTeamIndexChangedDelegate OnTeamChangedDelegate;

private:
	UFUNCTION()
	void OnRep_MyTeamID(FGenericTeamId OldTeamID);
};
