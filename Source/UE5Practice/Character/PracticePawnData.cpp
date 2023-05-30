// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticePawnData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticePawnData)

UPracticePawnData::UPracticePawnData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PawnClass = nullptr;
	InputConfig = nullptr;
	DefaultCameraMode = nullptr;
}

