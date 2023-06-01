// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "PracticeTestViewModel.generated.h"

/**
 * 
 */
UCLASS()
class PRACTICEGAME_API UPracticeTestViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	float Scale = 1.0f;
    
private:
	void SetScale(float Value)
	{
		UE_MVVM_SET_PROPERTY_VALUE(Scale, Value);
	}
	float GetScale() const
	{
		return Scale;
	}
};
