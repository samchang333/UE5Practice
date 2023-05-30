// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PracticeTeamInfoBase.h"

#include "PracticeTeamPrivateInfo.generated.h"

class UObject;

UCLASS()
class APracticeTeamPrivateInfo : public APracticeTeamInfoBase
{
	GENERATED_BODY()

public:
	APracticeTeamPrivateInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
