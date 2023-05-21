// Copyright 2020 Dan Kestranek.


#include "Tasks/MCV_AsyncTaskGameplayTagAddedRemoved.h"

UMCV_AsyncTaskGameplayTagAddedRemoved* UMCV_AsyncTaskGameplayTagAddedRemoved::ListenForGameplayTagAddedOrRemoved(UAbilitySystemComponent * AbilitySystemComponent, FGameplayTagContainer InTags)
{
	UMCV_AsyncTaskGameplayTagAddedRemoved* ListenForGameplayTagAddedRemoved = NewObject<UMCV_AsyncTaskGameplayTagAddedRemoved>();
	ListenForGameplayTagAddedRemoved->ASC = AbilitySystemComponent;
	ListenForGameplayTagAddedRemoved->Tags = InTags;

	if (!IsValid(AbilitySystemComponent) || InTags.Num() < 1)
	{
		ListenForGameplayTagAddedRemoved->EndTask();
		return nullptr;
	}

	TArray<FGameplayTag> TagArray;
	InTags.GetGameplayTagArray(TagArray);
	
	for (FGameplayTag Tag : TagArray)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).AddUObject(ListenForGameplayTagAddedRemoved, &UMCV_AsyncTaskGameplayTagAddedRemoved::TagChanged);
	}

	return ListenForGameplayTagAddedRemoved;
}

void UMCV_AsyncTaskGameplayTagAddedRemoved::EndTask()
{
	if (IsValid(ASC))
	{
		TArray<FGameplayTag> TagArray;
		Tags.GetGameplayTagArray(TagArray);

		for (FGameplayTag Tag : TagArray)
		{
			ASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
		}
	}

	SetReadyToDestroy();
	MarkPendingKill();
}

void UMCV_AsyncTaskGameplayTagAddedRemoved::TagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		OnTagAdded.Broadcast(Tag);
	}
	else
	{
		OnTagRemoved.Broadcast(Tag);
	}
}
