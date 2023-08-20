// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "PaperCharacter.h"
#include "GameFramework/Actor.h"
#include "FinePaperCharacter.generated.h"

class UAbilitySystemComponent;
class UFineCharacterGameplay;

/**
 * Base class for paper2d characters.
 */
UCLASS()
class FINEPLAY_API AFinePaperCharacter : public APaperCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFinePaperCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return AbilitySystemComponent;
	}

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(BlueprintReadOnly, Category = "FinePaperCharacter", meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "FinePaperCharacter", meta = (AllowPrivateAccess = "true"))
	UFineCharacterGameplay *CharacterGameplay;

	TSubclassOf<UFineCharacterGameplay> GameplayClass;
};
