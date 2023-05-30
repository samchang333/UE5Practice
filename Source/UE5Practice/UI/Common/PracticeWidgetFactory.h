// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/Object.h"

#include "PracticeWidgetFactory.generated.h"

template <class TClass> class TSubclassOf;

class UUserWidget;
struct FFrame;

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
class UE5PRACTICE_API UPracticeWidgetFactory : public UObject
{
	GENERATED_BODY()

public:
	UPracticeWidgetFactory() { }

	UFUNCTION(BlueprintNativeEvent)
	TSubclassOf<UUserWidget> FindWidgetClassForData(const UObject* Data) const;
};
