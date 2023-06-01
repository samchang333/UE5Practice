// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeWidgetFactory.h"
#include "Templates/SubclassOf.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeWidgetFactory)

class UUserWidget;

TSubclassOf<UUserWidget> UPracticeWidgetFactory::FindWidgetClassForData_Implementation(const UObject* Data) const
{
	return TSubclassOf<UUserWidget>();
}
