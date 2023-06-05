// Copyright 2022 David (Quod) Soler

#include "Components/MCV_AbilitySystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AttributeSets/MCV_AttributeSet.h"

void UMCV_AbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (AbilityActorInfo)
	{
		if (UGameInstance* GameInstance = InOwnerActor->GetGameInstance())
		{
			// Sign up for possess/unpossess events so that we can update the cached AbilityActorInfo accordingly
			GameInstance->GetOnPawnControllerChanged().AddDynamic(this, &UMCV_AbilitySystemComponent::OnPawnControllerChanged);
		}
	}

	GrantDefaultAbilitiesAndAttributes();
}

void UMCV_AbilitySystemComponent::BeginDestroy()
{
	if (AbilityActorInfo && AbilityActorInfo->OwnerActor.IsValid())
	{
		if (UGameInstance* GameInstance = AbilityActorInfo->OwnerActor->GetGameInstance())
		{
			GameInstance->GetOnPawnControllerChanged().RemoveAll(this);
		}
	}

	Super::BeginDestroy();
}

FGameplayAbilitySpecHandle UMCV_AbilitySystemComponent::GrantAbilityOfType(TSubclassOf<UGameplayAbility> AbilityType, bool bRemoveAfterActivation)
{
	FGameplayAbilitySpecHandle AbilityHandle;
	if (AbilityType)
	{
		FGameplayAbilitySpec AbilitySpec(AbilityType);
		AbilitySpec.RemoveAfterActivation = bRemoveAfterActivation;

		AbilityHandle = GiveAbility(AbilitySpec);
	}
	return AbilityHandle;
}

void UMCV_AbilitySystemComponent::K2_AddLooseGameplayTag(const FGameplayTag& GameplayTag, int32 Count /*= 1*/)
{
	AddLooseGameplayTag(GameplayTag, Count);
}

void UMCV_AbilitySystemComponent::K2_AddLooseGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count /*= 1*/)
{
	AddLooseGameplayTags(GameplayTags, Count);
}

void UMCV_AbilitySystemComponent::K2_RemoveLooseGameplayTag(const FGameplayTag& GameplayTag, int32 Count /*= 1*/)
{
	RemoveLooseGameplayTag(GameplayTag, Count);
}

void UMCV_AbilitySystemComponent::K2_RemoveLooseGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count /*= 1*/)
{
	RemoveLooseGameplayTags(GameplayTags, Count);
}

void UMCV_AbilitySystemComponent::GrantDefaultAbilitiesAndAttributes()
{
	// Reset/Remove abilities if we had already added them
	{
		for (UAttributeSet* AttribSetInstance : AddedAttributes)
		{
			GetSpawnedAttributes_Mutable().Remove(AttribSetInstance);
		}

		for (FGameplayAbilitySpecHandle AbilityHandle : DefaultAbilityHandles)
		{
			SetRemoveAbilityOnEnd(AbilityHandle);
		}

		AddedAttributes.Empty(DefaultAttributes.Num());
		DefaultAbilityHandles.Empty(DefaultAbilities.Num());
	}

	// Default abilities
	if(IsOwnerActorAuthoritative())
	{
		DefaultAbilityHandles.Reserve(DefaultAbilities.Num());
		for (const FMCVAbilityInitData& AbilityData : DefaultAbilities)
		{
			UClass* AbilityClass = AbilityData.Ability.Get();
			if (AbilityClass)
			{
				FGameplayAbilitySpec AbilitySpec(AbilityClass, 1, static_cast<int32>(AbilityData.InputID));
				DefaultAbilityHandles.Add(GiveAbility(AbilitySpec));
			}
		}
	

		// Default attributes
		{
			for (const FmcvAttributeApplication& Attributes : DefaultAttributes)
			{
				if (Attributes.AttributeSetType)
				{
					UAttributeSet* NewAttribSet = NewObject<UAttributeSet>(GetOwner(), Attributes.AttributeSetType);
					if (Attributes.InitializationData)
					{
						NewAttribSet->InitFromMetaDataTable(Attributes.InitializationData);
					}
					AddedAttributes.Add(NewAttribSet);
					AddAttributeSetSubobject(NewAttribSet);
				}
			}
		}

	}
}

void UMCV_AbilitySystemComponent::OnPawnControllerChanged(APawn* Pawn, AController* NewController)
{
	if (AbilityActorInfo && AbilityActorInfo->OwnerActor == Pawn && AbilityActorInfo->PlayerController != NewController)
	{
		// Reinit the cached ability actor info (specifically the player controller)
		AbilityActorInfo->InitFromActor(AbilityActorInfo->OwnerActor.Get(), AbilityActorInfo->AvatarActor.Get(), this);
	}
}
