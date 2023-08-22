// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FineActorGameplay.h"
#include "GameplayEffectTypes.h"
#include "UObject/Object.h"
#include "FineCharacterGameplay.generated.h"

struct FGameplayAbilitySpecHandle;
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

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "FineCharacterGameplay")
	void AddAbilityByClass(UClass* InClass, int32 InLevel, int32 InInputID);

	UPROPERTY(BlueprintAssignable)
	FOnCharacterDamageTaken OnCharacterDamageTaken;

	UFUNCTION(BlueprintCallable, Category = "FineCharacterGameplay")
	float GetDistanceFromGroundStaticMesh();
	
	UFUNCTION(BlueprintCallable, Category = "FineCharacterGameplay")
	FVector GetFeetLocation() const;

	UFUNCTION(BlueprintCallable, Category= "FineCharacterGameplay")
	void AddLooseGameplayTagForAbilitySystem(const FGameplayTag& Tag);
	UFUNCTION(BlueprintCallable, Category= "FineCharacterGameplay")
	void RemoveLooseGameplayTagForAbilitySystem(const FGameplayTag& Tag);
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineCharacterGameplay", meta = (AllowPrivateAccess = "true"))
	FName AliveGameplayTagName;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineCharacterGameplay", meta = (AllowPrivateAccess = "true"))
	FName InvincibleGameplayTagName;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineCharacterGameplay", meta = (AllowPrivateAccess = "true"))
	FName JumpGameplayTagName;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineCharacterGameplay", meta = (AllowPrivateAccess = "true"))
	FName RunGameplayTagName;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	FORCEINLINE void SetAttributeSetClass(const TSubclassOf<UFineCharacterAttributeSet>& InAttributeSetClass)
	{
		AttributeSetClass = InAttributeSetClass;
	}

	void OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnMovementSpeedChanged(const FOnAttributeChangeData& OnAttributeChangeData);

	void GiveDefaultAbilities();
	void ClearAllAbilities();

	FORCEINLINE UAbilitySystemComponent* GetAbilitySystemComponent() const
	{
		return AbilitySystemComponent.Get();
	}
private:
	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineCharacterGameplay", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UFineCharacterAttributeSet> AttributeSetClass;

	FDelegateHandle OnHealthUpdated;
	FDelegateHandle OnMovementSpeedUpdated;

	TArray<FGameplayAbilitySpecHandle> AbilityHandles;

public:
	// ------------------
	// Gameplay Attributes
	// ------------------
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetHealth() const;
	UFUNCTION(BlueprintCallable)
	void SetHealth(float InHealth);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(float InMaxHealth);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMana() const;
	UFUNCTION(BlueprintCallable)
	void SetMana(float InMana);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxMana() const;
	UFUNCTION(BlueprintCallable)
	void SetMaxMana(float InMaxMana);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMovementSpeed() const;
	UFUNCTION(BlueprintCallable)
	void SetMovementSpeed(float InMovementSpeed);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxMovementSpeed() const;
	UFUNCTION(BlueprintCallable)
	void SetMaxMovementSpeed(float InMaxMovementSpeed);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetAttackPower() const;
	UFUNCTION(BlueprintCallable)
	void SetAttackPower(float InAttackPower);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetDefensePower() const;
	UFUNCTION(BlueprintCallable)
	void SetDefensePower(float InDefensePower);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStamina() const;
	UFUNCTION(BlueprintCallable)
	void SetStamina(float InStamina);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxStamina() const;
	UFUNCTION(BlueprintCallable)
	void SetMaxStamina(float InMaxStamina);

	UFineCharacterAttributeSet* GetAttributeSet() const;
};
