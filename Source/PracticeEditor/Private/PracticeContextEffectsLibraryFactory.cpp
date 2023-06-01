// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeContextEffectsLibraryFactory.h"

#include "Feedback/ContextEffects/PracticeContextEffectsLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeContextEffectsLibraryFactory)

class FFeedbackContext;
class UClass;
class UObject;

UPracticeContextEffectsLibraryFactory::UPracticeContextEffectsLibraryFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UPracticeContextEffectsLibrary::StaticClass();

	bCreateNew = true;
	bEditorImport = false;
	bEditAfterNew = true;
}

UObject* UPracticeContextEffectsLibraryFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UPracticeContextEffectsLibrary* PracticeContextEffectsLibrary = NewObject<UPracticeContextEffectsLibrary>(InParent, Name, Flags);

	return PracticeContextEffectsLibrary;
}
