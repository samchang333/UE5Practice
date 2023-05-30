// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameSettingValueDiscrete.h"

#include "PracticeSettingValueDiscrete_PerfStat.generated.h"

enum class EPracticeDisplayablePerformanceStat : uint8;
enum class EPracticeStatDisplayMode : uint8;

class UObject;

UCLASS()
class UPracticeSettingValueDiscrete_PerfStat : public UGameSettingValueDiscrete
{
	GENERATED_BODY()
	
public:

	UPracticeSettingValueDiscrete_PerfStat();

	void SetStat(EPracticeDisplayablePerformanceStat InStat);

	/** UGameSettingValue */
	virtual void StoreInitial() override;
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;

	/** UGameSettingValueDiscrete */
	virtual void SetDiscreteOptionByIndex(int32 Index) override;
	virtual int32 GetDiscreteOptionIndex() const override;
	virtual TArray<FText> GetDiscreteOptions() const override;

protected:
	/** UGameSettingValue */
	virtual void OnInitialized() override;
	
	void AddMode(FText&& Label, EPracticeStatDisplayMode Mode);
protected:
	TArray<FText> Options;
	TArray<EPracticeStatDisplayMode> DisplayModes;

	EPracticeDisplayablePerformanceStat StatToDisplay;
	EPracticeStatDisplayMode InitialMode;
};
