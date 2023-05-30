// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeCharacter.h"

#include "AbilitySystem/PracticeAbilitySystemComponent.h"
#include "Camera/PracticeCameraComponent.h"
#include "Character/PracticeHealthComponent.h"
#include "Character/PracticePawnExtensionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "PracticeCharacterMovementComponent.h"
#include "PracticeGameplayTags.h"
#include "PracticeLogChannels.h"
#include "Net/UnrealNetwork.h"
#include "Player/PracticePlayerController.h"
#include "Player/PracticePlayerState.h"
#include "System/PracticeSignificanceManager.h"
#include "TimerManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeCharacter)

class AActor;
class FLifetimeProperty;
class IRepChangedPropertyTracker;
class UInputComponent;

static FName NAME_PracticeCharacterCollisionProfile_Capsule(TEXT("PracticePawnCapsule"));
static FName NAME_PracticeCharacterCollisionProfile_Mesh(TEXT("PracticePawnMesh"));

APracticeCharacter::APracticeCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPracticeCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Avoid ticking characters if possible.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	NetCullDistanceSquared = 900000000.0f;

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);
	CapsuleComp->SetCollisionProfileName(NAME_PracticeCharacterCollisionProfile_Capsule);

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));  // Rotate mesh to be X forward since it is exported as Y forward.
	MeshComp->SetCollisionProfileName(NAME_PracticeCharacterCollisionProfile_Mesh);

	UPracticeCharacterMovementComponent* PracticeMoveComp = CastChecked<UPracticeCharacterMovementComponent>(GetCharacterMovement());
	PracticeMoveComp->GravityScale = 1.0f;
	PracticeMoveComp->MaxAcceleration = 2400.0f;
	PracticeMoveComp->BrakingFrictionFactor = 1.0f;
	PracticeMoveComp->BrakingFriction = 6.0f;
	PracticeMoveComp->GroundFriction = 8.0f;
	PracticeMoveComp->BrakingDecelerationWalking = 1400.0f;
	PracticeMoveComp->bUseControllerDesiredRotation = false;
	PracticeMoveComp->bOrientRotationToMovement = false;
	PracticeMoveComp->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	PracticeMoveComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
	PracticeMoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	PracticeMoveComp->bCanWalkOffLedgesWhenCrouching = true;
	PracticeMoveComp->SetCrouchedHalfHeight(65.0f);

	PawnExtComponent = CreateDefaultSubobject<UPracticePawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

	HealthComponent = CreateDefaultSubobject<UPracticeHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

	CameraComponent = CreateDefaultSubobject<UPracticeCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;
}

void APracticeCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void APracticeCharacter::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	const bool bRegisterWithSignificanceManager = !IsNetMode(NM_DedicatedServer);
	if (bRegisterWithSignificanceManager)
	{
		if (UPracticeSignificanceManager* SignificanceManager = USignificanceManager::Get<UPracticeSignificanceManager>(World))
		{
//@TODO: SignificanceManager->RegisterObject(this, (EFortSignificanceType)SignificanceType);
		}
	}
}

void APracticeCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UWorld* World = GetWorld();

	const bool bRegisterWithSignificanceManager = !IsNetMode(NM_DedicatedServer);
	if (bRegisterWithSignificanceManager)
	{
		if (UPracticeSignificanceManager* SignificanceManager = USignificanceManager::Get<UPracticeSignificanceManager>(World))
		{
			SignificanceManager->UnregisterObject(this);
		}
	}
}

void APracticeCharacter::Reset()
{
	DisableMovementAndCollision();

	K2_OnReset();

	UninitAndDestroy();
}

void APracticeCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, ReplicatedAcceleration, COND_SimulatedOnly);
	DOREPLIFETIME(ThisClass, MyTeamID)
}

void APracticeCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		// Compress Acceleration: XY components as direction + magnitude, Z component as direct value
		const double MaxAccel = MovementComponent->MaxAcceleration;
		const FVector CurrentAccel = MovementComponent->GetCurrentAcceleration();
		double AccelXYRadians, AccelXYMagnitude;
		FMath::CartesianToPolar(CurrentAccel.X, CurrentAccel.Y, AccelXYMagnitude, AccelXYRadians);

		ReplicatedAcceleration.AccelXYRadians   = FMath::FloorToInt((AccelXYRadians / TWO_PI) * 255.0);     // [0, 2PI] -> [0, 255]
		ReplicatedAcceleration.AccelXYMagnitude = FMath::FloorToInt((AccelXYMagnitude / MaxAccel) * 255.0);	// [0, MaxAccel] -> [0, 255]
		ReplicatedAcceleration.AccelZ           = FMath::FloorToInt((CurrentAccel.Z / MaxAccel) * 127.0);   // [-MaxAccel, MaxAccel] -> [-127, 127]
	}
}

void APracticeCharacter::NotifyControllerChanged()
{
	const FGenericTeamId OldTeamId = GetGenericTeamId();

	Super::NotifyControllerChanged();

	// Update our team ID based on the controller
	if (HasAuthority() && (Controller != nullptr))
	{
		if (IPracticeTeamAgentInterface* ControllerWithTeam = Cast<IPracticeTeamAgentInterface>(Controller))
		{
			MyTeamID = ControllerWithTeam->GetGenericTeamId();
			ConditionalBroadcastTeamChanged(this, OldTeamId, MyTeamID);
		}
	}
}

APracticePlayerController* APracticeCharacter::GetPracticePlayerController() const
{
	return CastChecked<APracticePlayerController>(Controller, ECastCheckedType::NullAllowed);
}

APracticePlayerState* APracticeCharacter::GetPracticePlayerState() const
{
	return CastChecked<APracticePlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

UPracticeAbilitySystemComponent* APracticeCharacter::GetPracticeAbilitySystemComponent() const
{
	return Cast<UPracticeAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* APracticeCharacter::GetAbilitySystemComponent() const
{
	return PawnExtComponent->GetPracticeAbilitySystemComponent();
}

void APracticeCharacter::OnAbilitySystemInitialized()
{
	UPracticeAbilitySystemComponent* PracticeASC = GetPracticeAbilitySystemComponent();
	check(PracticeASC);

	HealthComponent->InitializeWithAbilitySystem(PracticeASC);

	InitializeGameplayTags();
}

void APracticeCharacter::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeFromAbilitySystem();
}

void APracticeCharacter::PossessedBy(AController* NewController)
{
	const FGenericTeamId OldTeamID = MyTeamID;

	Super::PossessedBy(NewController);

	PawnExtComponent->HandleControllerChanged();

	// Grab the current team ID and listen for future changes
	if (IPracticeTeamAgentInterface* ControllerAsTeamProvider = Cast<IPracticeTeamAgentInterface>(NewController))
	{
		MyTeamID = ControllerAsTeamProvider->GetGenericTeamId();
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnControllerChangedTeam);
	}
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void APracticeCharacter::UnPossessed()
{
	AController* const OldController = Controller;

	// Stop listening for changes from the old controller
	const FGenericTeamId OldTeamID = MyTeamID;
	if (IPracticeTeamAgentInterface* ControllerAsTeamProvider = Cast<IPracticeTeamAgentInterface>(OldController))
	{
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().RemoveAll(this);
	}

	Super::UnPossessed();

	PawnExtComponent->HandleControllerChanged();

	// Determine what the new team ID should be afterwards
	MyTeamID = DetermineNewTeamAfterPossessionEnds(OldTeamID);
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void APracticeCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	PawnExtComponent->HandleControllerChanged();
}

void APracticeCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PawnExtComponent->HandlePlayerStateReplicated();
}

void APracticeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtComponent->SetupPlayerInputComponent();
}

void APracticeCharacter::InitializeGameplayTags()
{
	// Clear tags that may be lingering on the ability system from the previous pawn.
	if (UPracticeAbilitySystemComponent* PracticeASC = GetPracticeAbilitySystemComponent())
	{
		for (const TPair<uint8, FGameplayTag>& TagMapping : PracticeGameplayTags::MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				PracticeASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		for (const TPair<uint8, FGameplayTag>& TagMapping : PracticeGameplayTags::CustomMovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				PracticeASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		UPracticeCharacterMovementComponent* PracticeMoveComp = CastChecked<UPracticeCharacterMovementComponent>(GetCharacterMovement());
		SetMovementModeTag(PracticeMoveComp->MovementMode, PracticeMoveComp->CustomMovementMode, true);
	}
}

void APracticeCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UPracticeAbilitySystemComponent* PracticeASC = GetPracticeAbilitySystemComponent())
	{
		PracticeASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool APracticeCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UPracticeAbilitySystemComponent* PracticeASC = GetPracticeAbilitySystemComponent())
	{
		return PracticeASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool APracticeCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UPracticeAbilitySystemComponent* PracticeASC = GetPracticeAbilitySystemComponent())
	{
		return PracticeASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool APracticeCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UPracticeAbilitySystemComponent* PracticeASC = GetPracticeAbilitySystemComponent())
	{
		return PracticeASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

void APracticeCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	HealthComponent->DamageSelfDestruct(/*bFellOutOfWorld=*/ true);
}

void APracticeCharacter::OnDeathStarted(AActor*)
{
	DisableMovementAndCollision();
}

void APracticeCharacter::OnDeathFinished(AActor*)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}


void APracticeCharacter::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UPracticeCharacterMovementComponent* PracticeMoveComp = CastChecked<UPracticeCharacterMovementComponent>(GetCharacterMovement());
	PracticeMoveComp->StopMovementImmediately();
	PracticeMoveComp->DisableMovement();
}

void APracticeCharacter::DestroyDueToDeath()
{
	K2_OnDeathFinished();

	UninitAndDestroy();
}


void APracticeCharacter::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (UPracticeAbilitySystemComponent* PracticeASC = GetPracticeAbilitySystemComponent())
	{
		if (PracticeASC->GetAvatarActor() == this)
		{
			PawnExtComponent->UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}

void APracticeCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	UPracticeCharacterMovementComponent* PracticeMoveComp = CastChecked<UPracticeCharacterMovementComponent>(GetCharacterMovement());

	SetMovementModeTag(PrevMovementMode, PreviousCustomMode, false);
	SetMovementModeTag(PracticeMoveComp->MovementMode, PracticeMoveComp->CustomMovementMode, true);
}

void APracticeCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
	if (UPracticeAbilitySystemComponent* PracticeASC = GetPracticeAbilitySystemComponent())
	{
		const FGameplayTag* MovementModeTag = nullptr;
		if (MovementMode == MOVE_Custom)
		{
			MovementModeTag = PracticeGameplayTags::CustomMovementModeTagMap.Find(CustomMovementMode);
		}
		else
		{
			MovementModeTag = PracticeGameplayTags::MovementModeTagMap.Find(MovementMode);
		}

		if (MovementModeTag && MovementModeTag->IsValid())
		{
			PracticeASC->SetLooseGameplayTagCount(*MovementModeTag, (bTagEnabled ? 1 : 0));
		}
	}
}

void APracticeCharacter::ToggleCrouch()
{
	const UPracticeCharacterMovementComponent* PracticeMoveComp = CastChecked<UPracticeCharacterMovementComponent>(GetCharacterMovement());

	if (bIsCrouched || PracticeMoveComp->bWantsToCrouch)
	{
		UnCrouch();
	}
	else if (PracticeMoveComp->IsMovingOnGround())
	{
		Crouch();
	}
}

void APracticeCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UPracticeAbilitySystemComponent* PracticeASC = GetPracticeAbilitySystemComponent())
	{
		PracticeASC->SetLooseGameplayTagCount(PracticeGameplayTags::Status_Crouching, 1);
	}


	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void APracticeCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UPracticeAbilitySystemComponent* PracticeASC = GetPracticeAbilitySystemComponent())
	{
		PracticeASC->SetLooseGameplayTagCount(PracticeGameplayTags::Status_Crouching, 0);
	}

	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

bool APracticeCharacter::CanJumpInternal_Implementation() const
{
	// same as ACharacter's implementation but without the crouch check
	return JumpIsAllowedInternal();
}

void APracticeCharacter::OnRep_ReplicatedAcceleration()
{
	if (UPracticeCharacterMovementComponent* PracticeMovementComponent = Cast<UPracticeCharacterMovementComponent>(GetCharacterMovement()))
	{
		// Decompress Acceleration
		const double MaxAccel         = PracticeMovementComponent->MaxAcceleration;
		const double AccelXYMagnitude = double(ReplicatedAcceleration.AccelXYMagnitude) * MaxAccel / 255.0; // [0, 255] -> [0, MaxAccel]
		const double AccelXYRadians   = double(ReplicatedAcceleration.AccelXYRadians) * TWO_PI / 255.0;     // [0, 255] -> [0, 2PI]

		FVector UnpackedAcceleration(FVector::ZeroVector);
		FMath::PolarToCartesian(AccelXYMagnitude, AccelXYRadians, UnpackedAcceleration.X, UnpackedAcceleration.Y);
		UnpackedAcceleration.Z = double(ReplicatedAcceleration.AccelZ) * MaxAccel / 127.0; // [-127, 127] -> [-MaxAccel, MaxAccel]

		PracticeMovementComponent->SetReplicatedAcceleration(UnpackedAcceleration);
	}
}

void APracticeCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (GetController() == nullptr)
	{
		if (HasAuthority())
		{
			const FGenericTeamId OldTeamID = MyTeamID;
			MyTeamID = NewTeamID;
			ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
		}
		else
		{
			UE_LOG(LogPracticeTeams, Error, TEXT("You can't set the team ID on a character (%s) except on the authority"), *GetPathNameSafe(this));
		}
	}
	else
	{
		UE_LOG(LogPracticeTeams, Error, TEXT("You can't set the team ID on a possessed character (%s); it's driven by the associated controller"), *GetPathNameSafe(this));
	}
}

FGenericTeamId APracticeCharacter::GetGenericTeamId() const
{
	return MyTeamID;
}

FOnPracticeTeamIndexChangedDelegate* APracticeCharacter::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

void APracticeCharacter::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	const FGenericTeamId MyOldTeamID = MyTeamID;
	MyTeamID = IntegerToGenericTeamId(NewTeam);
	ConditionalBroadcastTeamChanged(this, MyOldTeamID, MyTeamID);
}

void APracticeCharacter::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

bool APracticeCharacter::UpdateSharedReplication()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FSharedRepMovement SharedMovement;
		if (SharedMovement.FillForCharacter(this))
		{
			// Only call FastSharedReplication if data has changed since the last frame.
			// Skipping this call will cause replication to reuse the same bunch that we previously
			// produced, but not send it to clients that already received. (But a new client who has not received
			// it, will get it this frame)
			if (!SharedMovement.Equals(LastSharedReplication, this))
			{
				LastSharedReplication = SharedMovement;
				ReplicatedMovementMode = SharedMovement.RepMovementMode;

				FastSharedReplication(SharedMovement);
			}
			return true;
		}
	}

	// We cannot fastrep right now. Don't send anything.
	return false;
}

void APracticeCharacter::FastSharedReplication_Implementation(const FSharedRepMovement& SharedRepMovement)
{
	if (GetWorld()->IsPlayingReplay())
	{
		return;
	}

	// Timestamp is checked to reject old moves.
	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		// Timestamp
		ReplicatedServerLastTransformUpdateTimeStamp = SharedRepMovement.RepTimeStamp;

		// Movement mode
		if (ReplicatedMovementMode != SharedRepMovement.RepMovementMode)
		{
			ReplicatedMovementMode = SharedRepMovement.RepMovementMode;
			GetCharacterMovement()->bNetworkMovementModeChanged = true;
			GetCharacterMovement()->bNetworkUpdateReceived = true;
		}

		// Location, Rotation, Velocity, etc.
		FRepMovement& MutableRepMovement = GetReplicatedMovement_Mutable();
		MutableRepMovement = SharedRepMovement.RepMovement;

		// This also sets LastRepMovement
		OnRep_ReplicatedMovement();

		// Jump force
		bProxyIsJumpForceApplied = SharedRepMovement.bProxyIsJumpForceApplied;

		// Crouch
		if (bIsCrouched != SharedRepMovement.bIsCrouched)
		{
			bIsCrouched = SharedRepMovement.bIsCrouched;
			OnRep_IsCrouched();
		}
	}
}

FSharedRepMovement::FSharedRepMovement()
{
	RepMovement.LocationQuantizationLevel = EVectorQuantization::RoundTwoDecimals;
}

bool FSharedRepMovement::FillForCharacter(ACharacter* Character)
{
	if (USceneComponent* PawnRootComponent = Character->GetRootComponent())
	{
		UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement();

		RepMovement.Location = FRepMovement::RebaseOntoZeroOrigin(PawnRootComponent->GetComponentLocation(), Character);
		RepMovement.Rotation = PawnRootComponent->GetComponentRotation();
		RepMovement.LinearVelocity = CharacterMovement->Velocity;
		RepMovementMode = CharacterMovement->PackNetworkMovementMode();
		bProxyIsJumpForceApplied = Character->bProxyIsJumpForceApplied || (Character->JumpForceTimeRemaining > 0.0f);
		bIsCrouched = Character->bIsCrouched;

		// Timestamp is sent as zero if unused
		if ((CharacterMovement->NetworkSmoothingMode == ENetworkSmoothingMode::Linear) || CharacterMovement->bNetworkAlwaysReplicateTransformUpdateTimestamp)
		{
			RepTimeStamp = CharacterMovement->GetServerLastTransformUpdateTimeStamp();
		}
		else
		{
			RepTimeStamp = 0.f;
		}

		return true;
	}
	return false;
}

bool FSharedRepMovement::Equals(const FSharedRepMovement& Other, ACharacter* Character) const
{
	if (RepMovement.Location != Other.RepMovement.Location)
	{
		return false;
	}

	if (RepMovement.Rotation != Other.RepMovement.Rotation)
	{
		return false;
	}

	if (RepMovement.LinearVelocity != Other.RepMovement.LinearVelocity)
	{
		return false;
	}

	if (RepMovementMode != Other.RepMovementMode)
	{
		return false;
	}

	if (bProxyIsJumpForceApplied != Other.bProxyIsJumpForceApplied)
	{
		return false;
	}

	if (bIsCrouched != Other.bIsCrouched)
	{
		return false;
	}

	return true;
}

bool FSharedRepMovement::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;
	RepMovement.NetSerialize(Ar, Map, bOutSuccess);
	Ar << RepMovementMode;
	Ar << bProxyIsJumpForceApplied;
	Ar << bIsCrouched;

	// Timestamp, if non-zero.
	uint8 bHasTimeStamp = (RepTimeStamp != 0.f);
	Ar.SerializeBits(&bHasTimeStamp, 1);
	if (bHasTimeStamp)
	{
		Ar << RepTimeStamp;
	}
	else
	{
		RepTimeStamp = 0.f;
	}

	return true;
}
