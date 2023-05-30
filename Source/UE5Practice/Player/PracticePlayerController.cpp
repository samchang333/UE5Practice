// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticePlayerController.h"
#include "CommonInputTypeEnum.h"
#include "Components/PrimitiveComponent.h"
#include "PracticeLogChannels.h"
#include "PracticeCheatManager.h"
#include "PracticePlayerState.h"
#include "Camera/PracticePlayerCameraManager.h"
#include "UI/PracticeHUD.h"
#include "AbilitySystem/PracticeAbilitySystemComponent.h"
#include "EngineUtils.h"
#include "PracticeGameplayTags.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemGlobals.h"
#include "CommonInputSubsystem.h"
#include "PracticeLocalPlayer.h"
#include "Settings/PracticeSettingsShared.h"
#include "Development/PracticeDeveloperSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticePlayerController)

namespace Practice
{
	namespace Input
	{
		static int32 ShouldAlwaysPlayForceFeedback = 0;
		static FAutoConsoleVariableRef CVarShouldAlwaysPlayForceFeedback(TEXT("PracticePC.ShouldAlwaysPlayForceFeedback"),
			ShouldAlwaysPlayForceFeedback,
			TEXT("Should force feedback effects be played, even if the last input device was not a gamepad?"));
	}
}

APracticePlayerController::APracticePlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = APracticePlayerCameraManager::StaticClass();

#if USING_CHEAT_MANAGER
	CheatClass = UPracticeCheatManager::StaticClass();
#endif // #if USING_CHEAT_MANAGER
}

void APracticePlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void APracticePlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(false);
}

void APracticePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void APracticePlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
}

void APracticePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// If we are auto running then add some player input
	if (GetIsAutoRunning())
	{
		if (APawn* CurrentPawn = GetPawn())
		{
			const FRotator MovementRotation(0.0f, GetControlRotation().Yaw, 0.0f);
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			CurrentPawn->AddMovementInput(MovementDirection, 1.0f);	
		}
	}
}

APracticePlayerState* APracticePlayerController::GetPracticePlayerState() const
{
	return CastChecked<APracticePlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UPracticeAbilitySystemComponent* APracticePlayerController::GetPracticeAbilitySystemComponent() const
{
	const APracticePlayerState* PracticePS = GetPracticePlayerState();
	return (PracticePS ? PracticePS->GetPracticeAbilitySystemComponent() : nullptr);
}

APracticeHUD* APracticePlayerController::GetPracticeHUD() const
{
	return CastChecked<APracticeHUD>(GetHUD(), ECastCheckedType::NullAllowed);
}

void APracticePlayerController::OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	ConditionalBroadcastTeamChanged(this, IntegerToGenericTeamId(OldTeam), IntegerToGenericTeamId(NewTeam));
}

void APracticePlayerController::OnPlayerStateChanged()
{
	// Empty, place for derived classes to implement without having to hook all the other events
}

void APracticePlayerController::BroadcastOnPlayerStateChanged()
{
	OnPlayerStateChanged();

	// Unbind from the old player state, if any
	FGenericTeamId OldTeamID = FGenericTeamId::NoTeam;
	if (LastSeenPlayerState != nullptr)
	{
		if (IPracticeTeamAgentInterface* PlayerStateTeamInterface = Cast<IPracticeTeamAgentInterface>(LastSeenPlayerState))
		{
			OldTeamID = PlayerStateTeamInterface->GetGenericTeamId();
			PlayerStateTeamInterface->GetTeamChangedDelegateChecked().RemoveAll(this);
		}
	}

	// Bind to the new player state, if any
	FGenericTeamId NewTeamID = FGenericTeamId::NoTeam;
	if (PlayerState != nullptr)
	{
		if (IPracticeTeamAgentInterface* PlayerStateTeamInterface = Cast<IPracticeTeamAgentInterface>(PlayerState))
		{
			NewTeamID = PlayerStateTeamInterface->GetGenericTeamId();
			PlayerStateTeamInterface->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnPlayerStateChangedTeam);
		}
	}

	// Broadcast the team change (if it really has)
	ConditionalBroadcastTeamChanged(this, OldTeamID, NewTeamID);

	LastSeenPlayerState = PlayerState;
}

void APracticePlayerController::InitPlayerState()
{
	Super::InitPlayerState();
	BroadcastOnPlayerStateChanged();
}

void APracticePlayerController::CleanupPlayerState()
{
	Super::CleanupPlayerState();
	BroadcastOnPlayerStateChanged();
}

void APracticePlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	BroadcastOnPlayerStateChanged();
}

void APracticePlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);

	if (const UPracticeLocalPlayer* PracticeLocalPlayer = Cast<UPracticeLocalPlayer>(InPlayer))
	{
		UPracticeSettingsShared* UserSettings = PracticeLocalPlayer->GetSharedSettings();
		UserSettings->OnSettingChanged.AddUObject(this, &ThisClass::OnSettingsChanged);

		OnSettingsChanged(UserSettings);
	}
}

void APracticePlayerController::OnSettingsChanged(UPracticeSettingsShared* InSettings)
{
	bForceFeedbackEnabled = InSettings->GetForceFeedbackEnabled();
}

void APracticePlayerController::AddCheats(bool bForce)
{
#if USING_CHEAT_MANAGER
	Super::AddCheats(true);
#else //#if USING_CHEAT_MANAGER
	Super::AddCheats(bForce);
#endif // #else //#if USING_CHEAT_MANAGER
}

void APracticePlayerController::ServerCheat_Implementation(const FString& Msg)
{
#if USING_CHEAT_MANAGER
	if (CheatManager)
	{
		UE_LOG(LogPractice, Warning, TEXT("ServerCheat: %s"), *Msg);
		ClientMessage(ConsoleCommand(Msg));
	}
#endif // #if USING_CHEAT_MANAGER
}

bool APracticePlayerController::ServerCheat_Validate(const FString& Msg)
{
	return true;
}

void APracticePlayerController::ServerCheatAll_Implementation(const FString& Msg)
{
#if USING_CHEAT_MANAGER
	if (CheatManager)
	{
		UE_LOG(LogPractice, Warning, TEXT("ServerCheatAll: %s"), *Msg);
		for (TActorIterator<APracticePlayerController> It(GetWorld()); It; ++It)
		{
			APracticePlayerController* PracticePC = (*It);
			if (PracticePC)
			{
				PracticePC->ClientMessage(PracticePC->ConsoleCommand(Msg));
			}
		}
	}
#endif // #if USING_CHEAT_MANAGER
}

bool APracticePlayerController::ServerCheatAll_Validate(const FString& Msg)
{
	return true;
}

void APracticePlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
}

void APracticePlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UPracticeAbilitySystemComponent* PracticeASC = GetPracticeAbilitySystemComponent())
	{
		PracticeASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void APracticePlayerController::OnCameraPenetratingTarget()
{
	bHideViewTargetPawnNextFrame = true;
}

void APracticePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

#if WITH_SERVER_CODE && WITH_EDITOR
	if (GIsEditor && (InPawn != nullptr) && (GetPawn() == InPawn))
	{
		for (const FPracticeCheatToRun& CheatRow : GetDefault<UPracticeDeveloperSettings>()->CheatsToRun)
		{
			if (CheatRow.Phase == ECheatExecutionTime::OnPlayerPawnPossession)
			{
				ConsoleCommand(CheatRow.Cheat, /*bWriteToLog=*/ true);
			}
		}
	}
#endif

	SetIsAutoRunning(false);
}

void APracticePlayerController::SetIsAutoRunning(const bool bEnabled)
{
	const bool bIsAutoRunning = GetIsAutoRunning();
	if (bEnabled != bIsAutoRunning)
	{
		if (!bEnabled)
		{
			OnEndAutoRun();
		}
		else
		{
			OnStartAutoRun();
		}
	}
}

bool APracticePlayerController::GetIsAutoRunning() const
{
	bool bIsAutoRunning = false;
	if (const UPracticeAbilitySystemComponent* PracticeASC = GetPracticeAbilitySystemComponent())
	{
		bIsAutoRunning = PracticeASC->GetTagCount(PracticeGameplayTags::Status_AutoRunning) > 0;
	}
	return bIsAutoRunning;
}

void APracticePlayerController::OnStartAutoRun()
{
	if (UPracticeAbilitySystemComponent* PracticeASC = GetPracticeAbilitySystemComponent())
	{
		PracticeASC->SetLooseGameplayTagCount(PracticeGameplayTags::Status_AutoRunning, 1);
		K2_OnStartAutoRun();
	}	
}

void APracticePlayerController::OnEndAutoRun()
{
	if (UPracticeAbilitySystemComponent* PracticeASC = GetPracticeAbilitySystemComponent())
	{
		PracticeASC->SetLooseGameplayTagCount(PracticeGameplayTags::Status_AutoRunning, 0);
		K2_OnEndAutoRun();
	}
}

void APracticePlayerController::UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId)
{
	if (bForceFeedbackEnabled)
	{
		if (const UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(GetLocalPlayer()))
		{
			const ECommonInputType CurrentInputType = CommonInputSubsystem->GetCurrentInputType();
			if (Practice::Input::ShouldAlwaysPlayForceFeedback || CurrentInputType == ECommonInputType::Gamepad || CurrentInputType == ECommonInputType::Touch)
			{
				InputInterface->SetForceFeedbackChannelValues(ControllerId, ForceFeedbackValues);
				return;
			}
		}
	}
	
	InputInterface->SetForceFeedbackChannelValues(ControllerId, FForceFeedbackValues());
}

void APracticePlayerController::UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents)
{
	Super::UpdateHiddenComponents(ViewLocation, OutHiddenComponents);

	if (bHideViewTargetPawnNextFrame)
	{
		AActor* const ViewTargetPawn = PlayerCameraManager ? Cast<AActor>(PlayerCameraManager->GetViewTarget()) : nullptr;
		if (ViewTargetPawn)
		{
			// internal helper func to hide all the components
			auto AddToHiddenComponents = [&OutHiddenComponents](const TInlineComponentArray<UPrimitiveComponent*>& InComponents)
			{
				// add every component and all attached children
				for (UPrimitiveComponent* Comp : InComponents)
				{
					if (Comp->IsRegistered())
					{
						OutHiddenComponents.Add(Comp->ComponentId);

						for (USceneComponent* AttachedChild : Comp->GetAttachChildren())
						{
							static FName NAME_NoParentAutoHide(TEXT("NoParentAutoHide"));
							UPrimitiveComponent* AttachChildPC = Cast<UPrimitiveComponent>(AttachedChild);
							if (AttachChildPC && AttachChildPC->IsRegistered() && !AttachChildPC->ComponentTags.Contains(NAME_NoParentAutoHide))
							{
								OutHiddenComponents.Add(AttachChildPC->ComponentId);
							}
						}
					}
				}
			};

			//TODO Solve with an interface.  Gather hidden components or something.
			//TODO Hiding isn't awesome, sometimes you want the effect of a fade out over a proximity, needs to bubble up to designers.

			// hide pawn's components
			TInlineComponentArray<UPrimitiveComponent*> PawnComponents;
			ViewTargetPawn->GetComponents(PawnComponents);
			AddToHiddenComponents(PawnComponents);

			//// hide weapon too
			//if (ViewTargetPawn->CurrentWeapon)
			//{
			//	TInlineComponentArray<UPrimitiveComponent*> WeaponComponents;
			//	ViewTargetPawn->CurrentWeapon->GetComponents(WeaponComponents);
			//	AddToHiddenComponents(WeaponComponents);
			//}
		}

		// we consumed it, reset for next frame
		bHideViewTargetPawnNextFrame = false;
	}
}

void APracticePlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	UE_LOG(LogPracticeTeams, Error, TEXT("You can't set the team ID on a player controller (%s); it's driven by the associated player state"), *GetPathNameSafe(this));
}

FGenericTeamId APracticePlayerController::GetGenericTeamId() const
{
	if (const IPracticeTeamAgentInterface* PSWithTeamInterface = Cast<IPracticeTeamAgentInterface>(PlayerState))
	{
		return PSWithTeamInterface->GetGenericTeamId();
	}
	return FGenericTeamId::NoTeam;
}

FOnPracticeTeamIndexChangedDelegate* APracticePlayerController::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

void APracticePlayerController::OnUnPossess()
{
	// Make sure the pawn that is being unpossessed doesn't remain our ASC's avatar actor
	if (APawn* PawnBeingUnpossessed = GetPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
		{
			if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
			{
				ASC->SetAvatarActor(nullptr);
			}
		}
	}

	Super::OnUnPossess();
}

//////////////////////////////////////////////////////////////////////
// APracticeReplayPlayerController

void APracticeReplayPlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);
}

