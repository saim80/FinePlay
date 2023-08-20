// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "UObject/Object.h"
#include "AbilitySystemComponent.h"
#include "FineCharacterAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * Provides a common set of attributes that can be useful for any games.
 *
 * NOTE: Not considering networked games at the moment. Replication related operations are not implemented.
 */
UCLASS(Blueprintable, BlueprintType)
class FINEPLAY_API UFineCharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	ATTRIBUTE_ACCESSORS(UFineCharacterAttributeSet, Health)
	ATTRIBUTE_ACCESSORS(UFineCharacterAttributeSet, Mana)
	ATTRIBUTE_ACCESSORS(UFineCharacterAttributeSet, MovementSpeed)
	ATTRIBUTE_ACCESSORS(UFineCharacterAttributeSet, AttackPower)
	ATTRIBUTE_ACCESSORS(UFineCharacterAttributeSet, DefensePower)
	ATTRIBUTE_ACCESSORS(UFineCharacterAttributeSet, IncomingDamage)

	UPROPERTY(BlueprintReadOnly, Category = "AttributeSet")
	float MaxHealth;
	UPROPERTY(BlueprintReadOnly, Category = "AttributeSet")
	float MaxMana;
	UPROPERTY(BlueprintReadOnly, Category = "AttributeSet")
	float MaxMovementSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "AttributeSet")
	FGameplayAttributeData Health;
	UPROPERTY(BlueprintReadOnly, Category = "AttributeSet")
	FGameplayAttributeData Mana;
	UPROPERTY(BlueprintReadOnly, Category = "AttributeSet")
	FGameplayAttributeData MovementSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "AttributeSet")
	FGameplayAttributeData AttackPower;
	UPROPERTY(BlueprintReadOnly, Category = "AttributeSet")
	FGameplayAttributeData DefensePower;
	UPROPERTY(BlueprintReadOnly, Category = "AttributeSet")
	FGameplayAttributeData IncomingDamage;

protected:
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
};
