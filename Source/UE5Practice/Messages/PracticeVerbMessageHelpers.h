// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "PracticeVerbMessageHelpers.generated.h"

struct FGameplayCueParameters;
struct FPracticeVerbMessage;

class APlayerController;
class APlayerState;
class UObject;
struct FFrame;


UCLASS()
class UE5PRACTICE_API UPracticeVerbMessageHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Practice")
	static APlayerState* GetPlayerStateFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Practice")
	static APlayerController* GetPlayerControllerFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Practice")
	static FGameplayCueParameters VerbMessageToCueParameters(const FPracticeVerbMessage& Message);

	UFUNCTION(BlueprintCallable, Category = "Practice")
	static FPracticeVerbMessage CueParametersToVerbMessage(const FGameplayCueParameters& Params);
};
