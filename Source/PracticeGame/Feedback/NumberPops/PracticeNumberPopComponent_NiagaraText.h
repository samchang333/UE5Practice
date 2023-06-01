// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PracticeNumberPopComponent.h"

#include "PracticeNumberPopComponent_NiagaraText.generated.h"

class UPracticeDamagePopStyleNiagara;
class UNiagaraComponent;
class UObject;

UCLASS(Blueprintable)
class UPracticeNumberPopComponent_NiagaraText : public UPracticeNumberPopComponent
{
	GENERATED_BODY()

public:

	UPracticeNumberPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UPracticeNumberPopComponent interface
	virtual void AddNumberPop(const FPracticeNumberPopRequest& NewRequest) override;
	//~End of UPracticeNumberPopComponent interface

protected:
	
	TArray<int32> DamageNumberArray;

	/** Style patterns to attempt to apply to the incoming number pops */
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<UPracticeDamagePopStyleNiagara> Style;

	//Niagara Component used to display the damage
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<UNiagaraComponent> NiagaraComp;
};
