// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeRuntimeOptions.h"

#include "UObject/Class.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeRuntimeOptions)

UPracticeRuntimeOptions::UPracticeRuntimeOptions()
{
	OptionCommandPrefix = TEXT("ro");
}

UPracticeRuntimeOptions* UPracticeRuntimeOptions::GetRuntimeOptions()
{
	return GetMutableDefault<UPracticeRuntimeOptions>();
}

const UPracticeRuntimeOptions& UPracticeRuntimeOptions::Get()
{
	const UPracticeRuntimeOptions& RuntimeOptions = *GetDefault<UPracticeRuntimeOptions>();
	return RuntimeOptions;
}
