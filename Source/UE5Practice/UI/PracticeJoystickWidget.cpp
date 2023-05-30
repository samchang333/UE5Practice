// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/PracticeJoystickWidget.h"

#include "Components/Image.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeJoystickWidget)

#define LOCTEXT_NAMESPACE "PracticeJoystick"

UPracticeJoystickWidget::UPracticeJoystickWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetConsumePointerInput(true);
}

FReply UPracticeJoystickWidget::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	Super::NativeOnTouchStarted(InGeometry, InGestureEvent);
	
	TouchOrigin = InGestureEvent.GetScreenSpacePosition();

	FReply Reply = FReply::Handled();
	if (!HasMouseCaptureByUser(InGestureEvent.GetUserIndex(), InGestureEvent.GetPointerIndex()))
	{
		Reply.CaptureMouse(GetCachedWidget().ToSharedRef());
	}
	return Reply;
}

FReply UPracticeJoystickWidget::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	Super::NativeOnTouchMoved(InGeometry, InGestureEvent);
	HandleTouchDelta(InGeometry, InGestureEvent);

	FReply Reply = FReply::Handled();
	if (!HasMouseCaptureByUser(InGestureEvent.GetUserIndex(), InGestureEvent.GetPointerIndex()))
	{
		Reply.CaptureMouse(GetCachedWidget().ToSharedRef());
	}
	return Reply;
}

FReply UPracticeJoystickWidget::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	StopInputSimulation();
	return FReply::Handled().ReleaseMouseCapture();
}

void UPracticeJoystickWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	StopInputSimulation();
}

void UPracticeJoystickWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	// Move the inner stick icon around with the vector
	if (JoystickForeground && JoystickBackground)
	{
		JoystickForeground->SetRenderTranslation(
			(bNegateYAxis ? FVector2D(1.0f, -1.0f) : FVector2D(1.0f)) *
			StickVector *
			(JoystickBackground->GetDesiredSize() * 0.5f)
		);
	}
	
	InputKeyValue2D(StickVector);
}

void UPracticeJoystickWidget::HandleTouchDelta(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	const FVector2D& ScreenSpacePos = InGestureEvent.GetScreenSpacePosition();
	
	// The center of the geo locally is just its size
	FVector2D LocalStickCenter = InGeometry.GetAbsoluteSize();

	FVector2D ScreenSpaceStickCenter = InGeometry.LocalToAbsolute(LocalStickCenter);
	// Get the offset from the origin
	FVector2D MoveStickOffset = (ScreenSpacePos - ScreenSpaceStickCenter);
	if (bNegateYAxis)
	{
		MoveStickOffset *= FVector2D(1.0f, -1.0f);
	}
	
	FVector2D MoveStickDir = FVector2D::ZeroVector;
	float MoveStickLength = 0.0f;
	MoveStickOffset.ToDirectionAndLength(MoveStickDir, MoveStickLength);

	MoveStickLength = FMath::Min(MoveStickLength, StickRange);
	MoveStickOffset = MoveStickDir * MoveStickLength;

	StickVector = MoveStickOffset / StickRange;
}

void UPracticeJoystickWidget::StopInputSimulation()
{
	TouchOrigin = FVector2D::ZeroVector;
	StickVector = FVector2D::ZeroVector;
}

#undef LOCTEXT_NAMESPACE

