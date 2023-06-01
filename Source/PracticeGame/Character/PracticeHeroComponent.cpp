// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeHeroComponent.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Logging/MessageLog.h"
#include "Input/PracticeMappableConfigPair.h"
#include "PracticeLogChannels.h"
#include "EnhancedInputSubsystems.h"
#include "Player/PracticePlayerController.h"
#include "Player/PracticePlayerState.h"
#include "Player/PracticeLocalPlayer.h"
#include "Character/PracticePawnExtensionComponent.h"
#include "Character/PracticePawnData.h"
#include "Character/PracticeCharacter.h"
#include "AbilitySystem/PracticeAbilitySystemComponent.h"
#include "Input/PracticeInputConfig.h"
#include "Input/PracticeInputComponent.h"
#include "Camera/PracticeCameraComponent.h"
#include "PracticeGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include "PlayerMappableInputConfig.h"
#include "Camera/PracticeCameraMode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeHeroComponent)

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_EDITOR

namespace PracticeHero
{
	static const float LookYawRate = 300.0f;
	static const float LookPitchRate = 165.0f;
};

const FName UPracticeHeroComponent::NAME_BindInputsNow("BindInputsNow");
const FName UPracticeHeroComponent::NAME_ActorFeatureName("Hero");

UPracticeHeroComponent::UPracticeHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityCameraMode = nullptr;
	bReadyToBindInputs = false;
}

void UPracticeHeroComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		UE_LOG(LogPractice, Error, TEXT("[UPracticeHeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("PracticeHeroComponent", "NotOnPawnError", "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName HeroMessageLogName = TEXT("PracticeHeroComponent");
			
			FMessageLog(HeroMessageLogName).Error()
				->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
				->AddToken(FTextToken::Create(Message));
				
			FMessageLog(HeroMessageLogName).Open();
		}
#endif
	}
	else
	{
		// Register with the init state system early, this will only work if this is a game world
		RegisterInitStateFeature();
	}
}

bool UPracticeHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == PracticeGameplayTags::InitState_Spawned)
	{
		// As long as we have a real pawn, let us transition
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == PracticeGameplayTags::InitState_Spawned && DesiredState == PracticeGameplayTags::InitState_DataAvailable)
	{
		// The player state is required.
		if (!GetPlayerState<APracticePlayerState>())
		{
			return false;
		}

		// If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();

			const bool bHasControllerPairedWithPS = (Controller != nullptr) && \
				(Controller->PlayerState != nullptr) && \
				(Controller->PlayerState->GetOwner() == Controller);

			if (!bHasControllerPairedWithPS)
			{
				return false;
			}
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const bool bIsBot = Pawn->IsBotControlled();

		if (bIsLocallyControlled && !bIsBot)
		{
			APracticePlayerController* PracticePC = GetController<APracticePlayerController>();

			// The input component and local player is required when locally controlled.
			if (!Pawn->InputComponent || !PracticePC || !PracticePC->GetLocalPlayer())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == PracticeGameplayTags::InitState_DataAvailable && DesiredState == PracticeGameplayTags::InitState_DataInitialized)
	{
		// Wait for player state and extension component
		APracticePlayerState* PracticePS = GetPlayerState<APracticePlayerState>();

		return PracticePS && Manager->HasFeatureReachedInitState(Pawn, UPracticePawnExtensionComponent::NAME_ActorFeatureName, PracticeGameplayTags::InitState_DataInitialized);
	}
	else if (CurrentState == PracticeGameplayTags::InitState_DataInitialized && DesiredState == PracticeGameplayTags::InitState_GameplayReady)
	{
		// TODO add ability initialization checks?
		return true;
	}

	return false;
}

void UPracticeHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (CurrentState == PracticeGameplayTags::InitState_DataAvailable && DesiredState == PracticeGameplayTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		APracticePlayerState* PracticePS = GetPlayerState<APracticePlayerState>();
		if (!ensure(Pawn && PracticePS))
		{
			return;
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const UPracticePawnData* PawnData = nullptr;

		if (UPracticePawnExtensionComponent* PawnExtComp = UPracticePawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UPracticePawnData>();

			// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
			// The ability system component and attribute sets live on the player state.
			PawnExtComp->InitializeAbilitySystem(PracticePS->GetPracticeAbilitySystemComponent(), PracticePS);
		}

		if (APracticePlayerController* PracticePC = GetController<APracticePlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}

		if (bIsLocallyControlled && PawnData)
		{
			if (UPracticeCameraComponent* CameraComponent = UPracticeCameraComponent::FindCameraComponent(Pawn))
			{
				CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
			}
		}
	}
}

void UPracticeHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UPracticePawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == PracticeGameplayTags::InitState_DataInitialized)
		{
			// If the extension component says all all other components are initialized, try to progress to next state
			CheckDefaultInitialization();
		}
	}
}

void UPracticeHeroComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = { PracticeGameplayTags::InitState_Spawned, PracticeGameplayTags::InitState_DataAvailable, PracticeGameplayTags::InitState_DataInitialized, PracticeGameplayTags::InitState_GameplayReady };

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

void UPracticeHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for when the pawn extension component changes init state
	BindOnActorInitStateChanged(UPracticePawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// Notifies that we are done spawning, then try the rest of initialization
	ensure(TryToChangeInitState(PracticeGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UPracticeHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UPracticeHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const UPracticeLocalPlayer* LP = Cast<UPracticeLocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	if (const UPracticePawnExtensionComponent* PawnExtComp = UPracticePawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UPracticePawnData* PawnData = PawnExtComp->GetPawnData<UPracticePawnData>())
		{
			if (const UPracticeInputConfig* InputConfig = PawnData->InputConfig)
			{
				// Register any default input configs with the settings so that they will be applied to the player during AddInputMappings
				for (const FMappableConfigPair& Pair : DefaultInputConfigs)
				{
					if (Pair.bShouldActivateAutomatically && Pair.CanBeActivated())
					{
						FModifyContextOptions Options = {};
						Options.bIgnoreAllPressedKeysUntilRelease = false;
						// Actually add the config to the local player							
						Subsystem->AddPlayerMappableConfig(Pair.Config.LoadSynchronous(), Options);	
					}
				}

				// The Practice Input Component has some additional functions to map Gameplay Tags to an Input Action.
				// If you want this functionality but still want to change your input component class, make it a subclass
				// of the UPracticeInputComponent or modify this component accordingly.
				UPracticeInputComponent* PracticeIC = Cast<UPracticeInputComponent>(PlayerInputComponent);
				if (ensureMsgf(PracticeIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UPracticeInputComponent or a subclass of it.")))
				{
					// Add the key mappings that may have been set by the player
					PracticeIC->AddInputMappings(InputConfig, Subsystem);

					// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
					// be triggered directly by these input actions Triggered events. 
					TArray<uint32> BindHandles;
					PracticeIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

					PracticeIC->BindNativeAction(InputConfig, PracticeGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
					PracticeIC->BindNativeAction(InputConfig, PracticeGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
					PracticeIC->BindNativeAction(InputConfig, PracticeGameplayTags::InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
					PracticeIC->BindNativeAction(InputConfig, PracticeGameplayTags::InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
					PracticeIC->BindNativeAction(InputConfig, PracticeGameplayTags::InputTag_AutoRun, ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun, /*bLogIfNotFound=*/ false);
				}
			}
		}
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}
 
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UPracticeHeroComponent::AddAdditionalInputConfig(const UPracticeInputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}
	
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	if (const UPracticePawnExtensionComponent* PawnExtComp = UPracticePawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		UPracticeInputComponent* PracticeIC = Pawn->FindComponentByClass<UPracticeInputComponent>();
		if (ensureMsgf(PracticeIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UPracticeInputComponent or a subclass of it.")))
		{
			PracticeIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		}
	}
}

void UPracticeHeroComponent::RemoveAdditionalInputConfig(const UPracticeInputConfig* InputConfig)
{
	//@TODO: Implement me!
}

bool UPracticeHeroComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}

void UPracticeHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UPracticePawnExtensionComponent* PawnExtComp = UPracticePawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UPracticeAbilitySystemComponent* PracticeASC = PawnExtComp->GetPracticeAbilitySystemComponent())
			{
				PracticeASC->AbilityInputTagPressed(InputTag);
			}
		}	
	}
}

void UPracticeHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	if (const UPracticePawnExtensionComponent* PawnExtComp = UPracticePawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (UPracticeAbilitySystemComponent* PracticeASC = PawnExtComp->GetPracticeAbilitySystemComponent())
		{
			PracticeASC->AbilityInputTagReleased(InputTag);
		}
	}
}

void UPracticeHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	// If the player has attempted to move again then cancel auto running
	if (APracticePlayerController* PracticeController = Cast<APracticePlayerController>(Controller))
	{
		PracticeController->SetIsAutoRunning(false);
	}
	
	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UPracticeHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

void UPracticeHeroComponent::Input_LookStick(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	const UWorld* World = GetWorld();
	check(World);

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X * PracticeHero::LookYawRate * World->GetDeltaSeconds());
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y * PracticeHero::LookPitchRate * World->GetDeltaSeconds());
	}
}

void UPracticeHeroComponent::Input_Crouch(const FInputActionValue& InputActionValue)
{
	if (APracticeCharacter* Character = GetPawn<APracticeCharacter>())
	{
		Character->ToggleCrouch();
	}
}

void UPracticeHeroComponent::Input_AutoRun(const FInputActionValue& InputActionValue)
{
	if (APawn* Pawn = GetPawn<APawn>())
	{
		if (APracticePlayerController* Controller = Cast<APracticePlayerController>(Pawn->GetController()))
		{
			// Toggle auto running
			Controller->SetIsAutoRunning(!Controller->GetIsAutoRunning());
		}	
	}
}

TSubclassOf<UPracticeCameraMode> UPracticeHeroComponent::DetermineCameraMode() const
{
	if (AbilityCameraMode)
	{
		return AbilityCameraMode;
	}

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return nullptr;
	}

	if (UPracticePawnExtensionComponent* PawnExtComp = UPracticePawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UPracticePawnData* PawnData = PawnExtComp->GetPawnData<UPracticePawnData>())
		{
			return PawnData->DefaultCameraMode;
		}
	}

	return nullptr;
}

void UPracticeHeroComponent::SetAbilityCameraMode(TSubclassOf<UPracticeCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (CameraMode)
	{
		AbilityCameraMode = CameraMode;
		AbilityCameraModeOwningSpecHandle = OwningSpecHandle;
	}
}

void UPracticeHeroComponent::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (AbilityCameraModeOwningSpecHandle == OwningSpecHandle)
	{
		AbilityCameraMode = nullptr;
		AbilityCameraModeOwningSpecHandle = FGameplayAbilitySpecHandle();
	}
}

