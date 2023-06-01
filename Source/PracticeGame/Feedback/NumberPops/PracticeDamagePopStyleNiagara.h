// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "PracticeDamagePopStyleNiagara.generated.h"

class UNiagaraSystem;

/*PopStyle is used to define what Niagara asset should be used for the Damage System representation*/
UCLASS()
class UPracticeDamagePopStyleNiagara : public UDataAsset
{
	GENERATED_BODY()

public:

	//Name of the Niagra Array to set the Damage informations
	UPROPERTY(EditDefaultsOnly, Category="DamagePop")
	FName NiagaraArrayName;

	//Niagara System used to display the damages
	UPROPERTY(EditDefaultsOnly, Category="DamagePop")
	TObjectPtr<UNiagaraSystem> TextNiagara;
};
