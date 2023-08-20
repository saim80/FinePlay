// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FineActorGameplay.h"
#include "GameplayEffectTypes.h"
#include "UObject/Object.h"
#include "FineCharacterGameplay.generated.h"

class UFineCharacterAttributeSet;
class UAbilitySystemComponent;

// The actual damage done to the character after all calculations are done.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDamageTaken, float, Damage);

/**
 * Provides functionalities for managing character gameplay.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FINEPLAY_API UFineCharacterGameplay : public UFineActorGameplay
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFineCharacterGameplay();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAlive();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsInvincible();

	UPROPERTY(BlueprintAssignable)
	FOnCharacterDamageTaken OnCharacterDamageTaken;
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	FORCEINLINE void SetAttributeSetClass(const TSubclassOf<UFineCharacterAttributeSet>& InAttributeSetClass)
	{
		AttributeSetClass = InAttributeSetClass;
	}

	void OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnMovementSpeedChanged(const FOnAttributeChangeData& OnAttributeChangeData);

private:
	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineCharacterGameplay", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UFineCharacterAttributeSet> AttributeSetClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineCharacterGameplay", meta = (AllowPrivateAccess = "true"))
	FName AliveGameplayTagName;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineCharacterGameplay", meta = (AllowPrivateAccess = "true"))
	FName InvincibleGameplayTagName;

	FDelegateHandle OnHealthUpdated;
	FDelegateHandle OnMovementSpeedUpdated;

public:
	// ------------------
	// Gameplay Attributes
	// ------------------
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMana() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxMana() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMovementSpeed() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxMovementSpeed() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetAttackPower() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetDefensePower() const;

	UFineCharacterAttributeSet* GetAttributeSet() const;
};
