// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/CheatManager.h"

#include "PracticeBotCheats.generated.h"

class UPracticeBotCreationComponent;
class UObject;
struct FFrame;

/** Cheats related to bots */
UCLASS(NotBlueprintable)
class UPracticeBotCheats final : public UCheatManagerExtension
{
	GENERATED_BODY()

public:
	UPracticeBotCheats();

	// Adds a bot player
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void AddPlayerBot();

	// Removes a random bot player
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void RemovePlayerBot();

private:
	UPracticeBotCreationComponent* GetBotComponent() const;
};
