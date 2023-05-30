// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeTextHotfixConfig.h"
#include "Internationalization/PolyglotTextData.h"
#include "Internationalization/TextLocalizationManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeTextHotfixConfig)

UPracticeTextHotfixConfig::UPracticeTextHotfixConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPracticeTextHotfixConfig::ApplyTextReplacements() const
{
	FTextLocalizationManager::Get().RegisterPolyglotTextData(TextReplacements);
}

void UPracticeTextHotfixConfig::PostInitProperties()
{
	Super::PostInitProperties();
	ApplyTextReplacements();
}

void UPracticeTextHotfixConfig::PostReloadConfig(FProperty* PropertyThatWasLoaded)
{
	Super::PostReloadConfig(PropertyThatWasLoaded);
	ApplyTextReplacements();
}

#if WITH_EDITOR
void UPracticeTextHotfixConfig::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ApplyTextReplacements();
}
#endif

