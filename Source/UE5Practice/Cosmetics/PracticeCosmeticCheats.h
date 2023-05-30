// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/CheatManager.h"

#include "PracticeCosmeticCheats.generated.h"

class UPracticeControllerComponent_CharacterParts;
class UObject;
struct FFrame;

/** Cheats related to bots */
UCLASS(NotBlueprintable)
class UPracticeCosmeticCheats final : public UCheatManagerExtension
{
	GENERATED_BODY()

public:
	UPracticeCosmeticCheats();

	// Adds a character part
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void AddCharacterPart(const FString& AssetName, bool bSuppressNaturalParts = true);

	// Replaces previous cheat parts with a new one
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void ReplaceCharacterPart(const FString& AssetName, bool bSuppressNaturalParts = true);

	// Clears any existing cheats
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void ClearCharacterPartOverrides();

private:
	UPracticeControllerComponent_CharacterParts* GetCosmeticComponent() const;
};
