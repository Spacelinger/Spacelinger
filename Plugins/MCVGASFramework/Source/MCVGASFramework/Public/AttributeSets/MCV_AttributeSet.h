// Copyright 2022 David (Quod) Soler

#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "MCV_AttributeSet.generated.h"

// Useful for declaring attribute getters and setters
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * Base class for all attributes.
 * For now no extra functions, if wanted the attribute accessors can be declared
 * as an helper class.
 */
UCLASS(Abstract, EditInlineNew)
class MCVGASFRAMEWORK_API UMCV_AttributeSet : public UAttributeSet
{
	GENERATED_BODY()
};
