// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeBotCheats.h"
#include "Engine/World.h"
#include "GameFramework/CheatManagerDefines.h"
#include "GameModes/PracticeBotCreationComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeBotCheats)

//////////////////////////////////////////////////////////////////////
// UPracticeBotCheats

UPracticeBotCheats::UPracticeBotCheats()
{
#if WITH_SERVER_CODE && UE_WITH_CHEAT_MANAGER
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

void UPracticeBotCheats::AddPlayerBot()
{
#if WITH_SERVER_CODE && UE_WITH_CHEAT_MANAGER
	if (UPracticeBotCreationComponent* BotComponent = GetBotComponent())
	{
		BotComponent->Cheat_AddBot();
	}
#endif	
}

void UPracticeBotCheats::RemovePlayerBot()
{
#if WITH_SERVER_CODE && UE_WITH_CHEAT_MANAGER
	if (UPracticeBotCreationComponent* BotComponent = GetBotComponent())
	{
		BotComponent->Cheat_RemoveBot();
	}
#endif	
}

UPracticeBotCreationComponent* UPracticeBotCheats::GetBotComponent() const
{
	if (UWorld* World = GetWorld())
	{
		if (AGameStateBase* GameState = World->GetGameState())
		{
			return GameState->FindComponentByClass<UPracticeBotCreationComponent>();
		}
	}

	return nullptr;
}

