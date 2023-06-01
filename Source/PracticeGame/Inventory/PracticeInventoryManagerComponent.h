// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "PracticeInventoryManagerComponent.generated.h"

class UPracticeInventoryItemDefinition;
class UPracticeInventoryItemInstance;
class UPracticeInventoryManagerComponent;
class UObject;
struct FFrame;
struct FPracticeInventoryList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** A message when an item is added to the inventory */
USTRUCT(BlueprintType)
struct FPracticeInventoryChangeMessage
{
	GENERATED_BODY()

	//@TODO: Tag based names+owning actors for inventories instead of directly exposing the component?
	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<UActorComponent> InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UPracticeInventoryItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 Delta = 0;
};

/** A single entry in an inventory */
USTRUCT(BlueprintType)
struct FPracticeInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FPracticeInventoryEntry()
	{}

	FString GetDebugString() const;

private:
	friend FPracticeInventoryList;
	friend UPracticeInventoryManagerComponent;

	UPROPERTY()
	TObjectPtr<UPracticeInventoryItemInstance> Instance = nullptr;

	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;
};

/** List of inventory items */
USTRUCT(BlueprintType)
struct FPracticeInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	FPracticeInventoryList()
		: OwnerComponent(nullptr)
	{
	}

	FPracticeInventoryList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

	TArray<UPracticeInventoryItemInstance*> GetAllItems() const;

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FPracticeInventoryEntry, FPracticeInventoryList>(Entries, DeltaParms, *this);
	}

	UPracticeInventoryItemInstance* AddEntry(TSubclassOf<UPracticeInventoryItemDefinition> ItemClass, int32 StackCount);
	void AddEntry(UPracticeInventoryItemInstance* Instance);

	void RemoveEntry(UPracticeInventoryItemInstance* Instance);

private:
	void BroadcastChangeMessage(FPracticeInventoryEntry& Entry, int32 OldCount, int32 NewCount);

private:
	friend UPracticeInventoryManagerComponent;

private:
	// Replicated list of items
	UPROPERTY()
	TArray<FPracticeInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FPracticeInventoryList> : public TStructOpsTypeTraitsBase2<FPracticeInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};










/**
 * Manages an inventory
 */
UCLASS(BlueprintType)
class PRACTICEGAME_API UPracticeInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPracticeInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	bool CanAddItemDefinition(TSubclassOf<UPracticeInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	UPracticeInventoryItemInstance* AddItemDefinition(TSubclassOf<UPracticeInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void AddItemInstance(UPracticeInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void RemoveItemInstance(UPracticeInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure=false)
	TArray<UPracticeInventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure)
	UPracticeInventoryItemInstance* FindFirstItemStackByDefinition(TSubclassOf<UPracticeInventoryItemDefinition> ItemDef) const;

	int32 GetTotalItemCountByDefinition(TSubclassOf<UPracticeInventoryItemDefinition> ItemDef) const;
	bool ConsumeItemsByDefinition(TSubclassOf<UPracticeInventoryItemDefinition> ItemDef, int32 NumToConsume);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~End of UObject interface

private:
	UPROPERTY(Replicated)
	FPracticeInventoryList InventoryList;
};
