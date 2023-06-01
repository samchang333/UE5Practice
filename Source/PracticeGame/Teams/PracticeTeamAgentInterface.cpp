// Copyright Epic Games, Inc. All Rights Reserved.

#include "Teams/PracticeTeamAgentInterface.h"

#include "PracticeLogChannels.h"
#include "UObject/ScriptInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PracticeTeamAgentInterface)

UPracticeTeamAgentInterface::UPracticeTeamAgentInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void IPracticeTeamAgentInterface::ConditionalBroadcastTeamChanged(TScriptInterface<IPracticeTeamAgentInterface> This, FGenericTeamId OldTeamID, FGenericTeamId NewTeamID)
{
	if (OldTeamID != NewTeamID)
	{
		const int32 OldTeamIndex = GenericTeamIdToInteger(OldTeamID); 
		const int32 NewTeamIndex = GenericTeamIdToInteger(NewTeamID);

		UObject* ThisObj = This.GetObject();
		UE_LOG(LogPracticeTeams, Verbose, TEXT("%s assigned team %d"),  *GetPathNameSafe(ThisObj), NewTeamIndex);

		This.GetInterface()->GetTeamChangedDelegateChecked().Broadcast(ThisObj, OldTeamIndex, NewTeamIndex);
	}
}

