// (c) 2023 Pururum LLC. All rights reserved.


#include "Actor/FineCharacterAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"
#include "Actor/FineCharacterGameplay.h"

bool UFineCharacterAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	static FProperty* DamageProperty = GetIncomingDamageAttribute().GetUProperty();
	const FProperty* ModifiedProperty = Data.EvaluatedData.Attribute.GetUProperty();

	// Is Damage about to be applied?
	if (DamageProperty == ModifiedProperty)
	{
		// Check if the owner has the invincible tag
		const auto Tag = FGameplayTag::RequestGameplayTag(FName(TEXT("Actor.State.Invincible")));
		if (Data.Target.HasMatchingGameplayTag(Tag))
		{
			// Don't take damage while invincible
			return false;
		}
	}
	return true;
}

void UFineCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	static auto DamageProperty = GetIncomingDamageAttribute().GetUProperty();
	const FProperty* ModifiedProperty = Data.EvaluatedData.Attribute.GetUProperty();

	// What property was modified?
	if (DamageProperty == ModifiedProperty)
	{
		// Treat damage as minus health
		SetHealth(GetHealth() - GetIncomingDamage());
		SetIncomingDamage(0);
	}
}

void UFineCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute.GetUProperty() == GetHealthAttribute().GetUProperty())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0.0f, MaxHealth);
	}
	else if (Attribute.GetUProperty() == GetManaAttribute().GetUProperty())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0.0f, MaxMana);
	}
	else if (Attribute.GetUProperty() == GetMovementSpeedAttribute().GetUProperty())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0.0f, MaxMovementSpeed);
	}
}

void UFineCharacterAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue,
                                                     float NewValue)
{
}
