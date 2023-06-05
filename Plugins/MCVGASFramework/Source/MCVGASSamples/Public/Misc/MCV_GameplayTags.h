// Copyright 2022 David (Quod) Soler

#pragma once

#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

// Namespace to store tags
namespace MCV_GameplayTags
{
	// Abilities ---------------------------
	MCVGASSAMPLES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability);
	MCVGASSAMPLES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityFullbody);
	// Gameplay events ---------------------
	MCVGASSAMPLES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent);
	MCVGASSAMPLES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEventCharacterDied);
	// Gameplay cues   ---------------------
	MCVGASSAMPLES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue);
	// Gameplay effects --------------------
	MCVGASSAMPLES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect);
	// Data --------------------------------
	MCVGASSAMPLES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data);
	MCVGASSAMPLES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(DataCombatDamagePhysical);
	// State -------------------------------
	MCVGASSAMPLES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State);
	MCVGASSAMPLES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(StateCombatDead);
}
