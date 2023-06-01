// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeCosmeticCheats.h"
#include "Cosmetics/PracticeCharacterPartTypes.h"
#include "PracticeControllerComponent_CharacterParts.h"
#include "GameFramework/CheatManagerDefines.h"
#include "System/PracticeDevelopmentStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeCosmeticCheats)

//////////////////////////////////////////////////////////////////////
// UPracticeCosmeticCheats

UPracticeCosmeticCheats::UPracticeCosmeticCheats()
{
#if UE_WITH_CHEAT_MANAGER
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		UCheatManager::RegisterForOnCheatManagerCreated(FOnCheatManagerCreated::FDelegate::CreateLambda(
			[](UCheatManager* CheatManager)
			{
				CheatManager->AddCheatManagerExtension(NewObject<ThisClass>(CheatManager));
			}));
	}
#endif
}

void UPracticeCosmeticCheats::AddCharacterPart(const FString& AssetName, bool bSuppressNaturalParts)
{
#if UE_WITH_CHEAT_MANAGER
	if (UPracticeControllerComponent_CharacterParts* CosmeticComponent = GetCosmeticComponent())
	{
		TSubclassOf<AActor> PartClass = UPracticeDevelopmentStatics::FindClassByShortName<AActor>(AssetName);
		if (PartClass != nullptr)
		{
			FPracticeCharacterPart Part;
			Part.PartClass = PartClass;

			CosmeticComponent->AddCheatPart(Part, bSuppressNaturalParts);
		}
	}
#endif	
}

void UPracticeCosmeticCheats::ReplaceCharacterPart(const FString& AssetName, bool bSuppressNaturalParts)
{
	ClearCharacterPartOverrides();
	AddCharacterPart(AssetName, bSuppressNaturalParts);
}

void UPracticeCosmeticCheats::ClearCharacterPartOverrides()
{
#if UE_WITH_CHEAT_MANAGER
	if (UPracticeControllerComponent_CharacterParts* CosmeticComponent = GetCosmeticComponent())
	{
		CosmeticComponent->ClearCheatParts();
	}
#endif	
}

UPracticeControllerComponent_CharacterParts* UPracticeCosmeticCheats::GetCosmeticComponent() const
{
	if (APlayerController* PC = GetPlayerController())
	{
		return PC->FindComponentByClass<UPracticeControllerComponent_CharacterParts>();
	}

	return nullptr;
}

