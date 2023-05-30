// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/PracticeSimulatedInputWidget.h"
#include "EnhancedInputSubsystems.h"
#include "PracticeLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeSimulatedInputWidget)

#define LOCTEXT_NAMESPACE "PracticeSimulatedInputWidget"

UPracticeSimulatedInputWidget::UPracticeSimulatedInputWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetConsumePointerInput(true);
}

#if WITH_EDITOR
const FText UPracticeSimulatedInputWidget::GetPaletteCategory()
{
	return LOCTEXT("PalleteCategory", "Input");
}
#endif // WITH_EDITOR

void UPracticeSimulatedInputWidget::NativeConstruct()
{
	// TODO: We should query the key the simulate whenever the user rebinds a key and after the PlayerController has
	// had it's input initalized. Doing it here will always result in the fallback key being used because
	// the PC does not have any keys mapped in enhanced input.
	QueryKeyToSimulate();
	
	Super::NativeConstruct();
}

FReply UPracticeSimulatedInputWidget::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	FlushSimulatedInput();
	
	return Super::NativeOnTouchEnded(InGeometry, InGestureEvent);
}

UEnhancedInputLocalPlayerSubsystem* UPracticeSimulatedInputWidget::GetEnhancedInputSubsystem() const
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ULocalPlayer* LP = GetOwningLocalPlayer())
		{
			return LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		}
	}
	return nullptr;
}

UEnhancedPlayerInput* UPracticeSimulatedInputWidget::GetPlayerInput() const
{
	if (UEnhancedInputLocalPlayerSubsystem* System = GetEnhancedInputSubsystem())
	{
		return System->GetPlayerInput();
	}
	return nullptr;
}

void UPracticeSimulatedInputWidget::InputKeyValue(const FVector& Value)
{
	if (UEnhancedPlayerInput* Input = GetPlayerInput())
	{
		if(KeyToSimulate.IsValid())
		{
			FInputKeyParams Params;
			Params.Delta = Value;
			Params.Key = KeyToSimulate;
			Params.NumSamples = 1;
			Params.DeltaTime = GetWorld()->GetDeltaSeconds();
			Params.bIsGamepadOverride = KeyToSimulate.IsGamepadKey();
			
			Input->InputKey(Params);	
		}
	}
	else
	{
		UE_LOG(LogPractice, Error, TEXT("'%s' is attempting to simulate input but has no player input!"), *GetNameSafe(this));
	}
}

void UPracticeSimulatedInputWidget::InputKeyValue2D(const FVector2D& Value)
{
	InputKeyValue(FVector(Value.X, Value.Y, 0.0));
}

void UPracticeSimulatedInputWidget::FlushSimulatedInput()
{
	if (UEnhancedPlayerInput* Input = GetPlayerInput())
	{
		Input->FlushPressedKeys();
	}
}

void UPracticeSimulatedInputWidget::QueryKeyToSimulate()
{
	if (UEnhancedInputLocalPlayerSubsystem* System = GetEnhancedInputSubsystem())
	{
		TArray<FKey> Keys = System->QueryKeysMappedToAction(AssociatedAction);
		if(!Keys.IsEmpty() && Keys[0].IsValid())
		{
			KeyToSimulate = Keys[0];
		}
		else
		{
			KeyToSimulate = FallbackBindingKey;
		}
	}
}

#undef LOCTEXT_NAMESPACE
