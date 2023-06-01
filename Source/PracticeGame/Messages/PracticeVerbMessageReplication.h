// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "PracticeVerbMessage.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "PracticeVerbMessageReplication.generated.h"

class UObject;
struct FPracticeVerbMessageReplication;
struct FNetDeltaSerializeInfo;

/**
 * Represents one verb message
 */
USTRUCT(BlueprintType)
struct FPracticeVerbMessageReplicationEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FPracticeVerbMessageReplicationEntry()
	{}

	FPracticeVerbMessageReplicationEntry(const FPracticeVerbMessage& InMessage)
		: Message(InMessage)
	{
	}

	FString GetDebugString() const;

private:
	friend FPracticeVerbMessageReplication;

	UPROPERTY()
	FPracticeVerbMessage Message;
};

/** Container of verb messages to replicate */
USTRUCT(BlueprintType)
struct FPracticeVerbMessageReplication : public FFastArraySerializer
{
	GENERATED_BODY()

	FPracticeVerbMessageReplication()
	{
	}

public:
	void SetOwner(UObject* InOwner) { Owner = InOwner; }

	// Broadcasts a message from server to clients
	void AddMessage(const FPracticeVerbMessage& Message);

	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FPracticeVerbMessageReplicationEntry, FPracticeVerbMessageReplication>(CurrentMessages, DeltaParms, *this);
	}

private:
	void RebroadcastMessage(const FPracticeVerbMessage& Message);

private:
	// Replicated list of gameplay tag stacks
	UPROPERTY()
	TArray<FPracticeVerbMessageReplicationEntry> CurrentMessages;
	
	// Owner (for a route to a world)
	UPROPERTY()
	TObjectPtr<UObject> Owner = nullptr;
};

template<>
struct TStructOpsTypeTraits<FPracticeVerbMessageReplication> : public TStructOpsTypeTraitsBase2<FPracticeVerbMessageReplication>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
