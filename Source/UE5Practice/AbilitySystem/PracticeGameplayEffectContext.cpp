// Copyright Epic Games, Inc. All Rights Reserved.

#include "PracticeGameplayEffectContext.h"

#include "AbilitySystem/PracticeAbilitySourceInterface.h"
#include "Engine/HitResult.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationState/PropertyNetSerializerInfoRegistry.h"
#include "Serialization/GameplayEffectContextNetSerializer.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeGameplayEffectContext)

class FArchive;

FPracticeGameplayEffectContext* FPracticeGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FPracticeGameplayEffectContext::StaticStruct()))
	{
		return (FPracticeGameplayEffectContext*)BaseEffectContext;
	}

	return nullptr;
}

bool FPracticeGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	// Not serialized for post-activation use:
	// CartridgeID

	return true;
}

#if UE_WITH_IRIS
namespace UE::Net
{
	// Forward to FGameplayEffectContextNetSerializer
	// Note: If FPracticeGameplayEffectContext::NetSerialize() is modified, a custom NetSerializesr must be implemented as the current fallback will no longer be sufficient.
	UE_NET_IMPLEMENT_FORWARDING_NETSERIALIZER_AND_REGISTRY_DELEGATES(PracticeGameplayEffectContext, FGameplayEffectContextNetSerializer);
}
#endif

void FPracticeGameplayEffectContext::SetAbilitySource(const IPracticeAbilitySourceInterface* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
	//SourceLevel = InSourceLevel;
}

const IPracticeAbilitySourceInterface* FPracticeGameplayEffectContext::GetAbilitySource() const
{
	return Cast<IPracticeAbilitySourceInterface>(AbilitySourceObject.Get());
}

const UPhysicalMaterial* FPracticeGameplayEffectContext::GetPhysicalMaterial() const
{
	if (const FHitResult* HitResultPtr = GetHitResult())
	{
		return HitResultPtr->PhysMaterial.Get();
	}
	return nullptr;
}

