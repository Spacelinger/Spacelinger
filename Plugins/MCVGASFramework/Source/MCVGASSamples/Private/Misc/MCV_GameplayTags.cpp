// Copyright 2022 David (Quod) Soler

#include "Misc/MCV_GameplayTags.h"
#include "GameplayTagsManager.h"

#define DEFINE_TAG_C(TagName, Tag, Comment) FNativeGameplayTag TagName("MCVGASFramework", "MCVGASSamples", Tag, TEXT(Comment), ENativeGameplayTagToken::PRIVATE_USE_MACRO_INSTEAD);

namespace MCV_GameplayTags
{
	DEFINE_TAG_C(Ability, "Ability", "Category of tags used to describe a GameplayAbility.");
	DEFINE_TAG_C(AbilityFullbody, "Ability.Fullbody", "For abilities that block fullbody.");
	DEFINE_TAG_C(GameplayEvent, "GameplayEvent", "Category of tags used to trigger an ability.");
	DEFINE_TAG_C(GameplayEventCharacterDied, "GameplayEvent.Character.Died", "Fired when a character dies.");
	DEFINE_TAG_C(GameplayCue, "GameplayCue", "Category of tags used to trigger cosmetic, clientside events.");
	DEFINE_TAG_C(GameplayEffect, "GameplayEffect", "Category of tags used to describe a GameplayEffect.");
	DEFINE_TAG_C(Data, "Data", "To be used for data storing.");
	DEFINE_TAG_C(DataCombatDamagePhysical, "Data.Combat.Damage.Physical", "For abilities that block fullbody.");
	DEFINE_TAG_C(State, "State", "To mark gameplay state.");
	DEFINE_TAG_C(StateCombatDead, "State.Combat.Dead", "Tag to mark a character as dead.");
}
