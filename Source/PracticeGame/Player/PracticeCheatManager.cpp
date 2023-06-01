// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeCheatManager.h"
#include "GameFramework/Pawn.h"
#include "PracticePlayerController.h"
#include "PracticeDebugCameraController.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Console.h"
#include "GameFramework/HUD.h"
#include "System/PracticeAssetManager.h"
#include "System/PracticeGameData.h"
#include "PracticeGameplayTags.h"
#include "AbilitySystem/PracticeAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Character/PracticeHealthComponent.h"
#include "Character/PracticePawnExtensionComponent.h"
#include "System/PracticeSystemStatics.h"
#include "Development/PracticeDeveloperSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeCheatManager)

DEFINE_LOG_CATEGORY(LogPracticeCheat);

namespace PracticeCheat
{
	static const FName NAME_Fixed = FName(TEXT("Fixed"));
	
	static bool bEnableDebugCameraCycling = false;
	static FAutoConsoleVariableRef CVarEnableDebugCameraCycling(
		TEXT("PracticeCheat.EnableDebugCameraCycling"),
		bEnableDebugCameraCycling,
		TEXT("If true then you can cycle the debug camera while running the game."),
		ECVF_Cheat);

	static bool bStartInGodMode = false;
	static FAutoConsoleVariableRef CVarStartInGodMode(
		TEXT("PracticeCheat.StartInGodMode"),
		bStartInGodMode,
		TEXT("If true then the God cheat will be applied on begin play"),
		ECVF_Cheat);
};


UPracticeCheatManager::UPracticeCheatManager()
{
	DebugCameraControllerClass = APracticeDebugCameraController::StaticClass();
}

void UPracticeCheatManager::InitCheatManager()
{
	Super::InitCheatManager();

#if WITH_EDITOR
	if (GIsEditor)
	{
		APlayerController* PC = GetOuterAPlayerController();
		for (const FPracticeCheatToRun& CheatRow : GetDefault<UPracticeDeveloperSettings>()->CheatsToRun)
		{
			if (CheatRow.Phase == ECheatExecutionTime::OnCheatManagerCreated)
			{
				PC->ConsoleCommand(CheatRow.Cheat, /*bWriteToLog=*/ true);
			}
		}
	}
#endif

	if (PracticeCheat::bStartInGodMode)
	{
		God();	
	}
}

void UPracticeCheatManager::CheatOutputText(const FString& TextToOutput)
{
#if USING_CHEAT_MANAGER
	// Output to the console.
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->ViewportConsole)
	{
		GEngine->GameViewport->ViewportConsole->OutputText(TextToOutput);
	}

	// Output to log.
	UE_LOG(LogPracticeCheat, Display, TEXT("%s"), *TextToOutput);
#endif // USING_CHEAT_MANAGER
}

void UPracticeCheatManager::Cheat(const FString& Msg)
{
	if (APracticePlayerController* PracticePC = Cast<APracticePlayerController>(GetOuterAPlayerController()))
	{
		PracticePC->ServerCheat(Msg.Left(128));
	}
}

void UPracticeCheatManager::CheatAll(const FString& Msg)
{
	if (APracticePlayerController* PracticePC = Cast<APracticePlayerController>(GetOuterAPlayerController()))
	{
		PracticePC->ServerCheatAll(Msg.Left(128));
	}
}

void UPracticeCheatManager::PlayNextGame()
{
	UPracticeSystemStatics::PlayNextGame(this);
}

void UPracticeCheatManager::EnableDebugCamera()
{
	Super::EnableDebugCamera();
}

void UPracticeCheatManager::DisableDebugCamera()
{
	FVector DebugCameraLocation;
	FRotator DebugCameraRotation;

	ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	APlayerController* OriginalPC = nullptr;

	if (DebugCC)
	{
		OriginalPC = DebugCC->OriginalControllerRef;
		DebugCC->GetPlayerViewPoint(DebugCameraLocation, DebugCameraRotation);
	}

	Super::DisableDebugCamera();

	if (OriginalPC && OriginalPC->PlayerCameraManager && (OriginalPC->PlayerCameraManager->CameraStyle == PracticeCheat::NAME_Fixed))
	{
		OriginalPC->SetInitialLocationAndRotation(DebugCameraLocation, DebugCameraRotation);

		OriginalPC->PlayerCameraManager->ViewTarget.POV.Location = DebugCameraLocation;
		OriginalPC->PlayerCameraManager->ViewTarget.POV.Rotation = DebugCameraRotation;
		OriginalPC->PlayerCameraManager->PendingViewTarget.POV.Location = DebugCameraLocation;
		OriginalPC->PlayerCameraManager->PendingViewTarget.POV.Rotation = DebugCameraRotation;
	}
}

bool UPracticeCheatManager::InDebugCamera() const
{
	return (Cast<ADebugCameraController>(GetOuter()) ? true : false);
}

void UPracticeCheatManager::EnableFixedCamera()
{
	const ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	APlayerController* PC = (DebugCC ? ToRawPtr(DebugCC->OriginalControllerRef) : GetOuterAPlayerController());

	if (PC && PC->PlayerCameraManager)
	{
		PC->SetCameraMode(PracticeCheat::NAME_Fixed);
	}
}

void UPracticeCheatManager::DisableFixedCamera()
{
	const ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	APlayerController* PC = (DebugCC ? ToRawPtr(DebugCC->OriginalControllerRef) : GetOuterAPlayerController());

	if (PC && PC->PlayerCameraManager)
	{
		PC->SetCameraMode(NAME_Default);
	}
}

bool UPracticeCheatManager::InFixedCamera() const
{
	const ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	const APlayerController* PC = (DebugCC ? ToRawPtr(DebugCC->OriginalControllerRef) : GetOuterAPlayerController());

	if (PC && PC->PlayerCameraManager)
	{
		return (PC->PlayerCameraManager->CameraStyle == PracticeCheat::NAME_Fixed);
	}

	return false;
}

void UPracticeCheatManager::ToggleFixedCamera()
{
	if (InFixedCamera())
	{
		DisableFixedCamera();
	}
	else
	{
		EnableFixedCamera();
	}
}

void UPracticeCheatManager::CycleDebugCameras()
{
	if (!PracticeCheat::bEnableDebugCameraCycling)
	{
		return;
	}
	
	if (InDebugCamera())
	{
		EnableFixedCamera();
		DisableDebugCamera();
	}
	else if (InFixedCamera())
	{
		DisableFixedCamera();
		DisableDebugCamera();
	}
	else
	{
		EnableDebugCamera();
		DisableFixedCamera();
	}
}

void UPracticeCheatManager::CycleAbilitySystemDebug()
{
	APlayerController* PC = Cast<APlayerController>(GetOuterAPlayerController());

	if (PC && PC->MyHUD)
	{
		if (!PC->MyHUD->bShowDebugInfo || !PC->MyHUD->DebugDisplay.Contains(TEXT("AbilitySystem")))
		{
			PC->MyHUD->ShowDebug(TEXT("AbilitySystem"));
		}

		PC->ConsoleCommand(TEXT("AbilitySystem.Debug.NextCategory"));
	}
}

void UPracticeCheatManager::CancelActivatedAbilities()
{
	if (UPracticeAbilitySystemComponent* PracticeASC = GetPlayerAbilitySystemComponent())
	{
		const bool bReplicateCancelAbility = true;
		PracticeASC->CancelInputActivatedAbilities(bReplicateCancelAbility);
	}
}

void UPracticeCheatManager::AddTagToSelf(FString TagName)
{
	FGameplayTag Tag = PracticeGameplayTags::FindTagByString(TagName, true);
	if (Tag.IsValid())
	{
		if (UPracticeAbilitySystemComponent* PracticeASC = GetPlayerAbilitySystemComponent())
		{
			PracticeASC->AddDynamicTagGameplayEffect(Tag);
		}
	}
	else
	{
		UE_LOG(LogPracticeCheat, Display, TEXT("AddTagToSelf: Could not find any tag matching [%s]."), *TagName);
	}
}

void UPracticeCheatManager::RemoveTagFromSelf(FString TagName)
{
	FGameplayTag Tag = PracticeGameplayTags::FindTagByString(TagName, true);
	if (Tag.IsValid())
	{
		if (UPracticeAbilitySystemComponent* PracticeASC = GetPlayerAbilitySystemComponent())
		{
			PracticeASC->RemoveDynamicTagGameplayEffect(Tag);
		}
	}
	else
	{
		UE_LOG(LogPracticeCheat, Display, TEXT("RemoveTagFromSelf: Could not find any tag matching [%s]."), *TagName);
	}
}

void UPracticeCheatManager::DamageSelf(float DamageAmount)
{
	if (UPracticeAbilitySystemComponent* PracticeASC = GetPlayerAbilitySystemComponent())
	{
		ApplySetByCallerDamage(PracticeASC, DamageAmount);
	}
}

void UPracticeCheatManager::DamageTarget(float DamageAmount)
{
	if (APracticePlayerController* PracticePC = Cast<APracticePlayerController>(GetOuterAPlayerController()))
	{
		if (PracticePC->GetNetMode() == NM_Client)
		{
			// Automatically send cheat to server for convenience.
			PracticePC->ServerCheat(FString::Printf(TEXT("DamageTarget %.2f"), DamageAmount));
			return;
		}

		FHitResult TargetHitResult;
		AActor* TargetActor = GetTarget(PracticePC, TargetHitResult);

		if (UPracticeAbilitySystemComponent* PracticeTargetASC = Cast<UPracticeAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor)))
		{
			ApplySetByCallerDamage(PracticeTargetASC, DamageAmount);
		}
	}
}

void UPracticeCheatManager::ApplySetByCallerDamage(UPracticeAbilitySystemComponent* PracticeASC, float DamageAmount)
{
	check(PracticeASC);

	TSubclassOf<UGameplayEffect> DamageGE = UPracticeAssetManager::GetSubclass(UPracticeGameData::Get().DamageGameplayEffect_SetByCaller);
	FGameplayEffectSpecHandle SpecHandle = PracticeASC->MakeOutgoingSpec(DamageGE, 1.0f, PracticeASC->MakeEffectContext());

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(PracticeGameplayTags::SetByCaller_Damage, DamageAmount);
		PracticeASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void UPracticeCheatManager::HealSelf(float HealAmount)
{
	if (UPracticeAbilitySystemComponent* PracticeASC = GetPlayerAbilitySystemComponent())
	{
		ApplySetByCallerHeal(PracticeASC, HealAmount);
	}
}

void UPracticeCheatManager::HealTarget(float HealAmount)
{
	if (APracticePlayerController* PracticePC = Cast<APracticePlayerController>(GetOuterAPlayerController()))
	{
		FHitResult TargetHitResult;
		AActor* TargetActor = GetTarget(PracticePC, TargetHitResult);

		if (UPracticeAbilitySystemComponent* PracticeTargetASC = Cast<UPracticeAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor)))
		{
			ApplySetByCallerHeal(PracticeTargetASC, HealAmount);
		}
	}
}

void UPracticeCheatManager::ApplySetByCallerHeal(UPracticeAbilitySystemComponent* PracticeASC, float HealAmount)
{
	check(PracticeASC);

	TSubclassOf<UGameplayEffect> HealGE = UPracticeAssetManager::GetSubclass(UPracticeGameData::Get().HealGameplayEffect_SetByCaller);
	FGameplayEffectSpecHandle SpecHandle = PracticeASC->MakeOutgoingSpec(HealGE, 1.0f, PracticeASC->MakeEffectContext());

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(PracticeGameplayTags::SetByCaller_Heal, HealAmount);
		PracticeASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

UPracticeAbilitySystemComponent* UPracticeCheatManager::GetPlayerAbilitySystemComponent() const
{
	if (APracticePlayerController* PracticePC = Cast<APracticePlayerController>(GetOuterAPlayerController()))
	{
		return PracticePC->GetPracticeAbilitySystemComponent();
	}
	return nullptr;
}

void UPracticeCheatManager::DamageSelfDestruct()
{
	if (APracticePlayerController* PracticePC = Cast<APracticePlayerController>(GetOuterAPlayerController()))
	{
 		if (const UPracticePawnExtensionComponent* PawnExtComp = UPracticePawnExtensionComponent::FindPawnExtensionComponent(PracticePC->GetPawn()))
		{
			if (PawnExtComp->HasReachedInitState(PracticeGameplayTags::InitState_GameplayReady))
			{
				if (UPracticeHealthComponent* HealthComponent = UPracticeHealthComponent::FindHealthComponent(PracticePC->GetPawn()))
				{
					HealthComponent->DamageSelfDestruct();
				}
			}
		}
	}
}

void UPracticeCheatManager::God()
{
	if (APracticePlayerController* PracticePC = Cast<APracticePlayerController>(GetOuterAPlayerController()))
	{
		if (PracticePC->GetNetMode() == NM_Client)
		{
			// Automatically send cheat to server for convenience.
			PracticePC->ServerCheat(FString::Printf(TEXT("God")));
			return;
		}

		if (UPracticeAbilitySystemComponent* PracticeASC = PracticePC->GetPracticeAbilitySystemComponent())
		{
			const FGameplayTag Tag = PracticeGameplayTags::Cheat_GodMode;
			const bool bHasTag = PracticeASC->HasMatchingGameplayTag(Tag);

			if (bHasTag)
			{
				PracticeASC->RemoveDynamicTagGameplayEffect(Tag);
			}
			else
			{
				PracticeASC->AddDynamicTagGameplayEffect(Tag);
			}
		}
	}
}

void UPracticeCheatManager::UnlimitedHealth(int32 Enabled)
{
	if (UPracticeAbilitySystemComponent* PracticeASC = GetPlayerAbilitySystemComponent())
	{
		const FGameplayTag Tag = PracticeGameplayTags::Cheat_UnlimitedHealth;
		const bool bHasTag = PracticeASC->HasMatchingGameplayTag(Tag);

		if ((Enabled == -1) || ((Enabled > 0) && !bHasTag) || ((Enabled == 0) && bHasTag))
		{
			if (bHasTag)
			{
				PracticeASC->RemoveDynamicTagGameplayEffect(Tag);
			}
			else
			{
				PracticeASC->AddDynamicTagGameplayEffect(Tag);
			}
		}
	}
}

