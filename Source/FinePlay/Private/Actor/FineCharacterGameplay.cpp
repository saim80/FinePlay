// (c) 2023 Pururum LLC. All rights reserved.


#include "Actor/FineCharacterGameplay.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "FinePlayLog.h"
#include "Actor/FineCharacterAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "Data/FineDatabaseRecord.h"
#include "Data/FineLocalDatabaseComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values for this component's properties
UFineCharacterGameplay::UFineCharacterGameplay(): Super()
{
	AttributeSetClass = UFineCharacterAttributeSet::StaticClass();
	AliveGameplayTagName = TEXT("Actor.State.Alive");
	InvincibleGameplayTagName = TEXT("Actor.State.Invincible");
	JumpGameplayTagName = TEXT("Actor.State.Jumping");
	RunGameplayTagName = TEXT("Actor.State.Running");
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

FGameplayAbilitySpecHandle UFineCharacterGameplay::AddAbilityByClass(UClass* InClass, int32 InLevel, int32 InInputID,
                                                                     UObject* InSourceObject)
{
	// Add ability to ability system.
	const auto AbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	const TSubclassOf<UGameplayAbility> AbilityClass = InClass;
	const auto Handle = AbilitySystem->GiveAbility(
		FGameplayAbilitySpec(AbilityClass, InLevel, InInputID, IsValid(InSourceObject) ? InSourceObject : this));
	AbilityHandles.Add(Handle);

	FP_LOG("Gave ability: %s, %i, %i", *InClass->GetName(), InLevel, InInputID);
	return Handle;
}

void UFineCharacterGameplay::RemoveAbilityByClass(UClass* InClass)
{
	const auto AbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	const TSubclassOf<UGameplayAbility> AbilityClass = InClass;
	for (const auto AbilityHandle : AbilityHandles)
	{
		const auto AbilitySpec = AbilitySystem->FindAbilitySpecFromHandle(AbilityHandle);
		if (AbilitySpec->Ability->IsA(AbilityClass))
		{
			AbilitySystem->ClearAbility(AbilityHandle);
			AbilityHandles.Remove(AbilityHandle);
			FP_LOG("Removed ability: %s", *InClass->GetName());
			return;
		}
	}
}

float UFineCharacterGameplay::GetDistanceFromGroundStaticMesh(const FVector Offset)
{
	// Get distance from ground static mesh.
	static const float MaxDistance = 1000.0f;
	const auto Owner = GetOwner();
	const auto Capsule = Owner->FindComponentByClass<UCapsuleComponent>();
	const auto CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	const auto Start = Owner->GetActorLocation() + Offset;
	const auto ActorDownVector = Owner->GetActorUpVector() * -1.0f;
	const auto End = Start + ActorDownVector * MaxDistance;
	FHitResult HitResult;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(Owner);
	const auto bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult, Start, End, ECC_Visibility, CollisionQueryParams);
	if (bHit)
	{
		const auto FinalDistance = HitResult.Distance - CapsuleHalfHeight;
		FP_VERBOSE("Distance from ground static mesh: %f", FinalDistance);
		return FinalDistance;
	}
	FP_VERBOSE("Distance from ground static mesh: %f (Max)", MaxDistance);
	return MaxDistance;
}

FVector UFineCharacterGameplay::GetFeetLocation() const
{
	// get owner
	const auto Owner = GetOwner();
	// get capsule
	const auto Capsule = Owner->FindComponentByClass<UCapsuleComponent>();
	// get capsule half height
	const auto CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	// get actor location
	const auto ActorLocation = Owner->GetActorLocation();
	// get actor up vector
	const auto ActorUpVector = Owner->GetActorUpVector();
	// get feet location
	const auto FeetLocation = ActorLocation - ActorUpVector * CapsuleHalfHeight;
	return FeetLocation;
}

FVector UFineCharacterGameplay::GetHeadLocation() const
{
	// get owner
	const auto Owner = GetOwner();
	// get capsule
	const auto Capsule = Owner->FindComponentByClass<UCapsuleComponent>();
	// get capsule half height
	const auto CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	// get actor location
	const auto ActorLocation = Owner->GetActorLocation();
	// get actor up vector
	const auto ActorUpVector = Owner->GetActorUpVector();
	// get feet location
	const auto HeadLocation = ActorLocation + ActorUpVector * CapsuleHalfHeight;
	return HeadLocation;
}

void UFineCharacterGameplay::AddLooseGameplayTagForAbilitySystem(const FGameplayTag& Tag)
{
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->AddLooseGameplayTag(Tag);
	}
}

void UFineCharacterGameplay::RemoveLooseGameplayTagForAbilitySystem(const FGameplayTag& Tag)
{
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(Tag);
	}
}

void UFineCharacterGameplay::BeginPlay()
{
	Super::BeginPlay();

	// Get ability system by finding the component from the owner.
	const auto Owner = GetOwner();
	auto AbilitySystem = SetAndGetAbilitySystemComponent();
	
	check(IsValid(AbilitySystem));
	const auto AttributeSet = NewObject<UFineCharacterAttributeSet>(Owner, AttributeSetClass);
	AbilitySystem->AddSpawnedAttribute(AttributeSet);

	// Use local database component to get the record for the ability attribute set.
	const auto Database = GetLocalDatabaseComponent();
	if (ensure(IsValid(Database)))
	{
		// database record from local database component
		bool bSuccess = false;
		const auto Record = Database->GetRecordByName(
			TEXT("CharacterAttributeSet"), ActorName, bSuccess);
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
			AttributeSet->InitStamina(Record.FloatFields[TEXT("Stamina")]);
			AttributeSet->MaxStamina = Record.FloatFields[TEXT("Stamina")];
		}
	}

	GiveDefaultAbilities();

	AbilitySystem->AddLooseGameplayTag(
		FGameplayTag::RequestGameplayTag(FName(TEXT("Actor.State.Alive"))));

	// Add listener for health change.
	OnHealthUpdated = AbilitySystem->GetGameplayAttributeValueChangeDelegate(
		UFineCharacterAttributeSet::GetHealthAttribute()).AddUObject(
		this, &UFineCharacterGameplay::OnHealthChanged);

	// Add listener for movement speed change.
	OnMovementSpeedUpdated = AbilitySystem->GetGameplayAttributeValueChangeDelegate(
		UFineCharacterAttributeSet::GetMovementSpeedAttribute()).AddUObject(
		this, &UFineCharacterGameplay::OnMovementSpeedChanged);

	// Apply stamina refill effect to periodically refill stamina.
	const auto StaminaRefillClass = LoadClass<UGameplayEffect>(
		this,TEXT("/FinePlay/Ability/GE_Refill_Stamina.GE_Refill_Stamina_C"));
	AbilitySystem->ApplyGameplayEffectToSelf(StaminaRefillClass->GetDefaultObject<UGameplayEffect>(), 1.0f,
	                                                  AbilitySystem->MakeEffectContext());
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

void UFineCharacterGameplay::GiveDefaultAbilities()
{
	// Fetch the list of records from "GameplayAbility" entity for the current actor.
	const auto Database = GetLocalDatabaseComponent();
	if (ensure(IsValid(Database)))
	{
		bool bSuccess = false;
		const auto Records = Database->FilterRecords(
			TEXT("GameplayAbility"), FString::Printf(TEXT("Name = '%s'"), *ActorName.ToString()), bSuccess);
		if (bSuccess)
		{
			for (const auto& Record : Records)
			{
				const auto AbilityName = Record.StringFields.FindChecked(TEXT("AbilityName"));
				const auto AbilityRecord = Database->GetRecordByName(TEXT("AbilityData"), *AbilityName, bSuccess);
				if (!bSuccess) continue;
				const auto AbilityClassPath = AbilityRecord.StringFields[TEXT("AbilityClass")];
				const auto AbilityLevel = AbilityRecord.IntFields[TEXT("Level")];
				const auto InputID = AbilityRecord.IntFields[TEXT("InputID")];
				// Turn AbilityClassPath into TSubclass<UGameplayAbility>.
				const FSoftClassPath SoftClassPath(AbilityClassPath);
				const auto AbilityClass = SoftClassPath.TryLoadClass<UGameplayAbility>();

				if (IsValid(AbilityClass))
				{
					AddAbilityByClass(AbilityClass, AbilityLevel, InputID);
				}
				else
				{
					FP_ERROR("Invalid ability class path: %s", *AbilityClassPath);
				}
			}
		}
		else
		{
			FP_LOG("GameplayAbility fetch failed.");
		}
	}
}

float UFineCharacterGameplay::GetHealth() const
{
	return GetAttributeSet()->GetHealth();
}

void UFineCharacterGameplay::SetHealth(float InHealth)
{
	// if value changed, update the attribute set.
	if (GetHealth() != InHealth)
	{
		GetAttributeSet()->SetHealth(InHealth);
	}
}

float UFineCharacterGameplay::GetMaxHealth() const
{
	return GetAttributeSet()->GetMaxHealth();
}

void UFineCharacterGameplay::SetMaxHealth(float InMaxHealth)
{
	// if value changed, update the attribute set.
	if (GetMaxHealth() != InMaxHealth)
	{
		GetAttributeSet()->SetMaxHealth(InMaxHealth);
	}
}

float UFineCharacterGameplay::GetMana() const
{
	return GetAttributeSet()->GetMana();
}

void UFineCharacterGameplay::SetMana(float InMana)
{
	// if value changed, update the attribute set.
	if (GetMana() != InMana)
	{
		GetAttributeSet()->SetMana(InMana);
	}
}

float UFineCharacterGameplay::GetMaxMana() const
{
	return GetAttributeSet()->GetMaxMana();
}

void UFineCharacterGameplay::SetMaxMana(float InMaxMana)
{
	// if value changed, update the attribute set.
	if (GetMaxMana() != InMaxMana)
	{
		GetAttributeSet()->SetMaxMana(InMaxMana);
	}
}

float UFineCharacterGameplay::GetMovementSpeed() const
{
	return GetAttributeSet()->GetMovementSpeed();
}

void UFineCharacterGameplay::SetMovementSpeed(float InMovementSpeed)
{
	// if value changed, update the attribute set.
	if (GetMovementSpeed() != InMovementSpeed)
	{
		GetAttributeSet()->SetMovementSpeed(InMovementSpeed);
	}
}

float UFineCharacterGameplay::GetMaxMovementSpeed() const
{
	return GetAttributeSet()->GetMaxMovementSpeed();
}

void UFineCharacterGameplay::SetMaxMovementSpeed(float InMaxMovementSpeed)
{
	// if value changed, update the attribute set.
	if (GetMaxMovementSpeed() != InMaxMovementSpeed)
	{
		GetAttributeSet()->SetMaxMovementSpeed(InMaxMovementSpeed);
	}
}

float UFineCharacterGameplay::GetAttackPower() const
{
	return GetAttributeSet()->GetAttackPower();
}

void UFineCharacterGameplay::SetAttackPower(float InAttackPower)
{
	// if value changed, update the attribute set.
	if (GetAttackPower() != InAttackPower)
	{
		GetAttributeSet()->SetAttackPower(InAttackPower);
	}
}

float UFineCharacterGameplay::GetDefensePower() const
{
	return GetAttributeSet()->GetDefensePower();
}

void UFineCharacterGameplay::SetDefensePower(float InDefensePower)
{
	// if value changed, update the attribute set.
	if (GetDefensePower() != InDefensePower)
	{
		GetAttributeSet()->SetDefensePower(InDefensePower);
	}
}

float UFineCharacterGameplay::GetStamina() const
{
	return GetAttributeSet()->GetStamina();
}

void UFineCharacterGameplay::SetStamina(float InStamina)
{
	// if value changed, update the attribute set.
	if (GetStamina() != InStamina)
	{
		GetAttributeSet()->SetStamina(InStamina);
	}
}

float UFineCharacterGameplay::GetMaxStamina() const
{
	return GetAttributeSet()->GetMaxStamina();
}

void UFineCharacterGameplay::SetMaxStamina(float InMaxStamina)
{
	// if value changed, update the attribute set.
	if (GetMaxStamina() != InMaxStamina)
	{
		GetAttributeSet()->SetMaxStamina(InMaxStamina);
	}
}

UFineCharacterAttributeSet* UFineCharacterGameplay::GetAttributeSet() const
{
	if (!AbilitySystemComponent.IsValid())
	{
		return nullptr;
	}

	auto AttributeSet = const_cast<UAttributeSet*>(AbilitySystemComponent->GetAttributeSet(AttributeSetClass));
	return Cast<UFineCharacterAttributeSet>(AttributeSet);
}

void UFineCharacterGameplay::ClearAllAbilities()
{
	const auto AbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	for (const auto& Handle : AbilityHandles)
	{
		AbilitySystem->ClearAbility(Handle);
	}
	// Clear the ability handles.
	AbilityHandles.Empty();
	FP_LOG("All abilities cleared.");
}

UAbilitySystemComponent* UFineCharacterGameplay::SetAndGetAbilitySystemComponent()
{
	if (AbilitySystemComponent.IsValid())
	{
		return AbilitySystemComponent.Get();
	}
	const auto Owner = GetOwner();
	AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
	return AbilitySystemComponent.Get();
}
