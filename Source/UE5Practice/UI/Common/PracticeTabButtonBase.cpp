// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeTabButtonBase.h"

#include "CommonLazyImage.h"
#include "UI/Common/PracticeTabListWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeTabButtonBase)

class UObject;
struct FSlateBrush;

void UPracticeTabButtonBase::SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject)
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrushFromLazyDisplayAsset(LazyObject);
	}
}

void UPracticeTabButtonBase::SetIconBrush(const FSlateBrush& Brush)
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrush(Brush);
	}
}

void UPracticeTabButtonBase::SetTabLabelInfo_Implementation(const FPracticeTabDescriptor& TabLabelInfo)
{
	SetButtonText(TabLabelInfo.TabText);
	SetIconBrush(TabLabelInfo.IconBrush);
}

