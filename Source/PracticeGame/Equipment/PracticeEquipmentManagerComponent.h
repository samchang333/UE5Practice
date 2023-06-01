// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystem/PracticeAbilitySet.h"
#include "Components/PawnComponent.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "PracticeEquipmentManagerComponent.generated.h"

class UActorComponent;
class UPracticeAbilitySystemComponent;
class UPracticeEquipmentDefinition;
class UPracticeEquipmentInstance;
class UPracticeEquipmentManagerComponent;
class UObject;
struct FFrame;
struct FPracticeEquipmentList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** A single piece of applied equipment */
USTRUCT(BlueprintType)
struct FPracticeAppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FPracticeAppliedEquipmentEntry()
	{}

	FString GetDebugString() const;

private:
	friend FPracticeEquipmentList;
	friend UPracticeEquipmentManagerComponent;

	// The equipment class that got equipped
	UPROPERTY()
	TSubclassOf<UPracticeEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<UPracticeEquipmentInstance> Instance = nullptr;

	// Authority-only list of granted handles
	UPROPERTY(NotReplicated)
	FPracticeAbilitySet_GrantedHandles GrantedHandles;
};

/** List of applied equipment */
USTRUCT(BlueprintType)
struct FPracticeEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FPracticeEquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	FPracticeEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FPracticeAppliedEquipmentEntry, FPracticeEquipmentList>(Entries, DeltaParms, *this);
	}

	UPracticeEquipmentInstance* AddEntry(TSubclassOf<UPracticeEquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(UPracticeEquipmentInstance* Instance);

private:
	UPracticeAbilitySystemComponent* GetAbilitySystemComponent() const;

	friend UPracticeEquipmentManagerComponent;

private:
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FPracticeAppliedEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FPracticeEquipmentList> : public TStructOpsTypeTraitsBase2<FPracticeEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};










/**
 * Manages equipment applied to a pawn
 */
UCLASS(BlueprintType, Const)
class UPracticeEquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UPracticeEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UPracticeEquipmentInstance* EquipItem(TSubclassOf<UPracticeEquipmentDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void UnequipItem(UPracticeEquipmentInstance* ItemInstance);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of UObject interface

	//~UActorComponent interface
	//virtual void EndPlay() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;
	//~End of UActorComponent interface

	/** Returns the first equipped instance of a given type, or nullptr if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UPracticeEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UPracticeEquipmentInstance> InstanceType);

 	/** Returns all equipped instances of a given type, or an empty array if none are found */
 	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UPracticeEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UPracticeEquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

private:
	UPROPERTY(Replicated)
	FPracticeEquipmentList EquipmentList;
};
