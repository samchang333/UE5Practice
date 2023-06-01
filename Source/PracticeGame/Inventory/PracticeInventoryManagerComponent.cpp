// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeInventoryManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "PracticeInventoryItemDefinition.h"
#include "PracticeInventoryItemInstance.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeInventoryManagerComponent)

class FLifetimeProperty;
struct FReplicationFlags;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Practice_Inventory_Message_StackChanged, "Practice.Inventory.Message.StackChanged");

//////////////////////////////////////////////////////////////////////
// FPracticeInventoryEntry

FString FPracticeInventoryEntry::GetDebugString() const
{
	TSubclassOf<UPracticeInventoryItemDefinition> ItemDef;
	if (Instance != nullptr)
	{
		ItemDef = Instance->GetItemDef();
	}

	return FString::Printf(TEXT("%s (%d x %s)"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
}

//////////////////////////////////////////////////////////////////////
// FPracticeInventoryList

void FPracticeInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		FPracticeInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.StackCount, /*NewCount=*/ 0);
		Stack.LastObservedCount = 0;
	}
}

void FPracticeInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		FPracticeInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ 0, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FPracticeInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		FPracticeInventoryEntry& Stack = Entries[Index];
		check(Stack.LastObservedCount != INDEX_NONE);
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.LastObservedCount, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FPracticeInventoryList::BroadcastChangeMessage(FPracticeInventoryEntry& Entry, int32 OldCount, int32 NewCount)
{
	FPracticeInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.Instance = Entry.Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSystem.BroadcastMessage(TAG_Practice_Inventory_Message_StackChanged, Message);
}

UPracticeInventoryItemInstance* FPracticeInventoryList::AddEntry(TSubclassOf<UPracticeInventoryItemDefinition> ItemDef, int32 StackCount)
{
	UPracticeInventoryItemInstance* Result = nullptr;

	check(ItemDef != nullptr);
 	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());


	FPracticeInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UPracticeInventoryItemInstance>(OwnerComponent->GetOwner());  //@TODO: Using the actor instead of component as the outer due to UE-127172
	NewEntry.Instance->SetItemDef(ItemDef);
	for (UPracticeInventoryItemFragment* Fragment : GetDefault<UPracticeInventoryItemDefinition>(ItemDef)->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}
	NewEntry.StackCount = StackCount;
	Result = NewEntry.Instance;

	//const UPracticeInventoryItemDefinition* ItemCDO = GetDefault<UPracticeInventoryItemDefinition>(ItemDef);
	MarkItemDirty(NewEntry);

	return Result;
}

void FPracticeInventoryList::AddEntry(UPracticeInventoryItemInstance* Instance)
{
	unimplemented();
}

void FPracticeInventoryList::RemoveEntry(UPracticeInventoryItemInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FPracticeInventoryEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

TArray<UPracticeInventoryItemInstance*> FPracticeInventoryList::GetAllItems() const
{
	TArray<UPracticeInventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());
	for (const FPracticeInventoryEntry& Entry : Entries)
	{
		if (Entry.Instance != nullptr) //@TODO: Would prefer to not deal with this here and hide it further?
		{
			Results.Add(Entry.Instance);
		}
	}
	return Results;
}

//////////////////////////////////////////////////////////////////////
// UPracticeInventoryManagerComponent

UPracticeInventoryManagerComponent::UPracticeInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
	SetIsReplicatedByDefault(true);
}

void UPracticeInventoryManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

bool UPracticeInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<UPracticeInventoryItemDefinition> ItemDef, int32 StackCount)
{
	//@TODO: Add support for stack limit / uniqueness checks / etc...
	return true;
}

UPracticeInventoryItemInstance* UPracticeInventoryManagerComponent::AddItemDefinition(TSubclassOf<UPracticeInventoryItemDefinition> ItemDef, int32 StackCount)
{
	UPracticeInventoryItemInstance* Result = nullptr;
	if (ItemDef != nullptr)
	{
		Result = InventoryList.AddEntry(ItemDef, StackCount);
		
		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && Result)
		{
			AddReplicatedSubObject(Result);
		}
	}
	return Result;
}

void UPracticeInventoryManagerComponent::AddItemInstance(UPracticeInventoryItemInstance* ItemInstance)
{
	InventoryList.AddEntry(ItemInstance);
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && ItemInstance)
	{
		AddReplicatedSubObject(ItemInstance);
	}
}

void UPracticeInventoryManagerComponent::RemoveItemInstance(UPracticeInventoryItemInstance* ItemInstance)
{
	InventoryList.RemoveEntry(ItemInstance);

	if (ItemInstance && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(ItemInstance);
	}
}

TArray<UPracticeInventoryItemInstance*> UPracticeInventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

UPracticeInventoryItemInstance* UPracticeInventoryManagerComponent::FindFirstItemStackByDefinition(TSubclassOf<UPracticeInventoryItemDefinition> ItemDef) const
{
	for (const FPracticeInventoryEntry& Entry : InventoryList.Entries)
	{
		UPracticeInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

int32 UPracticeInventoryManagerComponent::GetTotalItemCountByDefinition(TSubclassOf<UPracticeInventoryItemDefinition> ItemDef) const
{
	int32 TotalCount = 0;
	for (const FPracticeInventoryEntry& Entry : InventoryList.Entries)
	{
		UPracticeInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				++TotalCount;
			}
		}
	}

	return TotalCount;
}

bool UPracticeInventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<UPracticeInventoryItemDefinition> ItemDef, int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}

	//@TODO: N squared right now as there's no acceleration structure
	int32 TotalConsumed = 0;
	while (TotalConsumed < NumToConsume)
	{
		if (UPracticeInventoryItemInstance* Instance = UPracticeInventoryManagerComponent::FindFirstItemStackByDefinition(ItemDef))
		{
			InventoryList.RemoveEntry(Instance);
			++TotalConsumed;
		}
		else
		{
			return false;
		}
	}

	return TotalConsumed == NumToConsume;
}

void UPracticeInventoryManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing UPracticeInventoryItemInstance
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FPracticeInventoryEntry& Entry : InventoryList.Entries)
		{
			UPracticeInventoryItemInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

bool UPracticeInventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FPracticeInventoryEntry& Entry : InventoryList.Entries)
	{
		UPracticeInventoryItemInstance* Instance = Entry.Instance;

		if (Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

//////////////////////////////////////////////////////////////////////
//

// UCLASS(Abstract)
// class UPracticeInventoryFilter : public UObject
// {
// public:
// 	virtual bool PassesFilter(UPracticeInventoryItemInstance* Instance) const { return true; }
// };

// UCLASS()
// class UPracticeInventoryFilter_HasTag : public UPracticeInventoryFilter
// {
// public:
// 	virtual bool PassesFilter(UPracticeInventoryItemInstance* Instance) const { return true; }
// };


