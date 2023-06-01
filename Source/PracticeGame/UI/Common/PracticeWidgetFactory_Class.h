// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PracticeWidgetFactory.h"
#include "Templates/SubclassOf.h"
#include "UObject/SoftObjectPtr.h"

#include "PracticeWidgetFactory_Class.generated.h"

class UObject;
class UUserWidget;

UCLASS()
class PRACTICEGAME_API UPracticeWidgetFactory_Class : public UPracticeWidgetFactory
{
	GENERATED_BODY()

public:
	UPracticeWidgetFactory_Class() { }

	virtual TSubclassOf<UUserWidget> FindWidgetClassForData_Implementation(const UObject* Data) const override;
	
protected:
	UPROPERTY(EditAnywhere, Category = ListEntries, meta = (AllowAbstract))
	TMap<TSoftClassPtr<UObject>, TSubclassOf<UUserWidget>> EntryWidgetForClass;
};
