// (c) 2023 Pururum LLC. All rights reserved.


#include "Actor/FineCharacterAttributeSet.h"

#include "FinePlayLog.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"
#include "Actor/FineCharacterGameplay.h"
#include "Control/FineMovementInputControl.h"
#include "GameFramework/Character.h"

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
	static auto StaminaProperty = GetStaminaAttribute().GetUProperty();
	const FProperty* ModifiedProperty = Data.EvaluatedData.Attribute.GetUProperty();

	// What property was modified?
	if (DamageProperty == ModifiedProperty)
	{
		// Treat damage as minus health
		SetHealth(GetHealth() - GetIncomingDamage());
		SetIncomingDamage(0);
	}
	else if (StaminaProperty == ModifiedProperty)
	{
		// Cancel sprint if stamina is depleted
		if (GetStamina() <= 0)
		{
			// Get the character's ability system component, and cancel the sprint ability
			const auto AbilitySystem = GetOwningAbilitySystemComponent();
			if (ensure(IsValid(AbilitySystem)))
			{
				// Get movement input control from player controller.
				const auto AvatarActor = Cast<ACharacter>(AbilitySystem->GetAvatarActor());
				const auto PlayerController = Cast<APlayerController>(AvatarActor->GetController());
				if (!ensure(IsValid(PlayerController)))
				{
					return;
				}
				const auto MovementInputControl = PlayerController->FindComponentByClass<UFineMovementInputControl>();
				const auto SprintAbility = AbilitySystem->FindAbilitySpecFromInputID(
					MovementInputControl->GetRunActionInputID());
				if (IsValid(SprintAbility->Ability))
				{
					AbilitySystem->CancelAbility(SprintAbility->Ability);
				}
			}
		}
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
	else if (Attribute.GetUProperty() == GetStaminaAttribute().GetUProperty())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0.0f, MaxStamina);
	}
}

void UFineCharacterAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue,
                                                     float NewValue)
{
	if (Attribute.GetUProperty() == GetStaminaAttribute().GetUProperty())
	{
		static const auto ExhaustedTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Actor.State.Exhausted")));
		const auto AbilitySystem = GetOwningAbilitySystemComponent();
		if (FMath::IsNearlyEqual(NewValue, MaxStamina, UE_KINDA_SMALL_NUMBER))
		{
			if (AbilitySystem->HasMatchingGameplayTag(ExhaustedTag))
			{
				AbilitySystem->RemoveLooseGameplayTag(ExhaustedTag);
				FP_LOG("ExhaustedTag removed");
			}
		}
		else
		{
			if (!AbilitySystem->HasMatchingGameplayTag(ExhaustedTag))
			{
				AbilitySystem->AddLooseGameplayTag(ExhaustedTag);
				FP_LOG("ExhaustedTag added");
			}
		}
	}
}
