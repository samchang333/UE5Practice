// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeWeaponUserInterface.h"

#include "Equipment/PracticeEquipmentManagerComponent.h"
#include "GameFramework/Pawn.h"
#include "Weapons/PracticeWeaponInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeWeaponUserInterface)

struct FGeometry;

UPracticeWeaponUserInterface::UPracticeWeaponUserInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPracticeWeaponUserInterface::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPracticeWeaponUserInterface::NativeDestruct()
{
	Super::NativeDestruct();
}

void UPracticeWeaponUserInterface::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		if (UPracticeEquipmentManagerComponent* EquipmentManager = Pawn->FindComponentByClass<UPracticeEquipmentManagerComponent>())
		{
			if (UPracticeWeaponInstance* NewInstance = EquipmentManager->GetFirstInstanceOfType<UPracticeWeaponInstance>())
			{
				if (NewInstance != CurrentInstance && NewInstance->GetInstigator() != nullptr)
				{
					UPracticeWeaponInstance* OldWeapon = CurrentInstance;
					CurrentInstance = NewInstance;
					RebuildWidgetFromWeapon();
					OnWeaponChanged(OldWeapon, CurrentInstance);
				}
			}
		}
	}
}

void UPracticeWeaponUserInterface::RebuildWidgetFromWeapon()
{
	
}

