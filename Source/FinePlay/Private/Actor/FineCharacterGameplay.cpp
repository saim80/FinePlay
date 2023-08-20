// (c) 2023 Pururum LLC. All rights reserved.


#include "Actor/FineCharacterGameplay.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/FineCharacterAttributeSet.h"
#include "Data/FineDatabaseRecord.h"
#include "Data/FineLocalDatabaseComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values for this component's properties
UFineCharacterGameplay::UFineCharacterGameplay(): Super()
{
	AttributeSetClass = UFineCharacterAttributeSet::StaticClass();
	AliveGameplayTagName = TEXT("Actor.State.Alive");
	InvincibleGameplayTagName = TEXT("Actor.State.Invincible");
}

bool UFineCharacterGameplay::IsAlive()
{
	// Requires Actor.State.Invincible gameplay tag
	static const auto Tag = FGameplayTag::RequestGameplayTag(AliveGameplayTagName);
	return AbilitySystemComponent->HasMatchingGameplayTag(Tag);
}

bool UFineCharacterGameplay::IsInvincible()
{
	// Requires Actor.State.Invincible gameplay tag
	static const auto Tag = FGameplayTag::RequestGameplayTag(InvincibleGameplayTagName);
	return AbilitySystemComponent->HasMatchingGameplayTag(Tag);
}

void UFineCharacterGameplay::BeginPlay()
{
	Super::BeginPlay();

	// Get ability system by finding the component from the owner.
	const auto Owner = GetOwner();
	AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);

	check(IsValid(AbilitySystemComponent));
	auto AttributeSet = const_cast<UFineCharacterAttributeSet*>(AbilitySystemComponent->AddSet<
		UFineCharacterAttributeSet>());

	// Use local database component to get the record for the ability attribute set.
	const auto Database = GetLocalDatabaseComponent();
	if (ensure(IsValid(Database)))
	{
		// database record from local database component
		bool bSuccess = false;
		const auto Record = Database->GetRecordByName(
			TEXT("AbilityAttributeSet"), ActorName, bSuccess);
		if (bSuccess)
		{
			AttributeSet->InitHealth(Record.FloatFields[TEXT("Health")]);
			AttributeSet->MaxHealth = Record.FloatFields[TEXT("Health")];
			AttributeSet->InitMana(Record.FloatFields[TEXT("Mana")]);
			AttributeSet->MaxMana = Record.FloatFields[TEXT("Mana")];
			AttributeSet->InitMovementSpeed(Record.FloatFields[TEXT("MovementSpeed")]);
			AttributeSet->MaxMovementSpeed = 1000.f;
			AttributeSet->InitAttackPower(Record.FloatFields[TEXT("AttackPower")]);
			AttributeSet->InitDefensePower(Record.FloatFields[TEXT("DefensePower")]);
		}
	}
	AbilitySystemComponent->AddLooseGameplayTag(
		FGameplayTag::RequestGameplayTag(FName(TEXT("Actor.State.Alive"))));

	// Add listener for health change.
	OnHealthUpdated = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UFineCharacterAttributeSet::GetHealthAttribute()).AddUObject(
		this, &UFineCharacterGameplay::OnHealthChanged);

	// Add listener for movement speed change.
	OnMovementSpeedUpdated = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UFineCharacterAttributeSet::GetMovementSpeedAttribute()).AddUObject(
		this, &UFineCharacterGameplay::OnMovementSpeedChanged);
}

void UFineCharacterGameplay::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// remove listener for health change.
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UFineCharacterAttributeSet::GetHealthAttribute()).Remove(OnHealthUpdated);
	AbilitySystemComponent = nullptr;
	Super::EndPlay(EndPlayReason);
}

void UFineCharacterGameplay::OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	static const auto AliveTag = FGameplayTag::RequestGameplayTag(AliveGameplayTagName);
	if (OnAttributeChangeData.OldValue > 0 && OnAttributeChangeData.NewValue <= 0)
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(AliveTag);
	}
	else if (OnAttributeChangeData.OldValue <= 0 && OnAttributeChangeData.NewValue > 0)
	{
		AbilitySystemComponent->AddLooseGameplayTag(AliveTag);
	}
	const auto DamageDone = OnAttributeChangeData.OldValue - OnAttributeChangeData.NewValue;
	if (DamageDone > 0.0f)
	{
		OnCharacterDamageTaken.Broadcast(DamageDone);
	}
}

void UFineCharacterGameplay::OnMovementSpeedChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	// Get character movement component from the owner.
	const auto Owner = GetOwner();
	const auto CharacterMovementComponent = Owner->FindComponentByClass<UCharacterMovementComponent>();
	if (ensure(IsValid(CharacterMovementComponent)))
	{
		CharacterMovementComponent->MaxWalkSpeed = OnAttributeChangeData.NewValue;
	}
}

float UFineCharacterGameplay::GetHealth() const
{
	return GetAttributeSet()->GetHealth();
}

float UFineCharacterGameplay::GetMaxHealth() const
{
	return GetAttributeSet()->MaxHealth;
}

float UFineCharacterGameplay::GetMana() const
{
	return GetAttributeSet()->GetMana();
}

float UFineCharacterGameplay::GetMaxMana() const
{
	return GetAttributeSet()->MaxMana;
}

float UFineCharacterGameplay::GetMovementSpeed() const
{
	return GetAttributeSet()->GetMovementSpeed();
}

float UFineCharacterGameplay::GetMaxMovementSpeed() const
{
	return GetAttributeSet()->MaxMovementSpeed;
}

float UFineCharacterGameplay::GetAttackPower() const
{
	return GetAttributeSet()->GetAttackPower();
}

float UFineCharacterGameplay::GetDefensePower() const
{
	return GetAttributeSet()->GetDefensePower();
}

UFineCharacterAttributeSet* UFineCharacterGameplay::GetAttributeSet() const
{
	auto AttributeSet = const_cast<UAttributeSet*>(AbilitySystemComponent->GetAttributeSet(AttributeSetClass));
	return Cast<UFineCharacterAttributeSet>(AttributeSet);
}
