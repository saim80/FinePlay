// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Data/FineActorData.h"
#include "UObject/Object.h"
#include "FineActorGameplay.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthUpdated, AActor*, Actor, int32, NewHealth, int32, OldHealth);

class UFineLocalDatabaseComponent;
/**
 * Basic gameplay for common actors
 */
UCLASS(Category = Gameplay, meta = (BlueprintSpawnableComponent))
class FINEPLAY_API UFineActorGameplay : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnHealthUpdated OnHealthUpdated;

	FORCEINLINE const FFineActorData& GetActorData() const { return ActorData; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	void SetHealth(int32 NewHealth);

	FORCEINLINE UFineLocalDatabaseComponent* GetLocalDatabaseComponent() const { return LocalDatabaseComponent; }
	FORCEINLINE const FGameplayTagContainer& GetGameplayTags() { return GameplayTags; }

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineActorGameplay")
	FString EntityName;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineActorGameplay")
	FName ActorName;

private:
	UPROPERTY(BlueprintReadOnly, Category = "FineActorGameplay", meta = (AllowPrivateAccess = "true"))
	FFineActorData ActorData;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 Health;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FineActorGameplay", meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer GameplayTags;

	UPROPERTY()
	TObjectPtr<UFineLocalDatabaseComponent> LocalDatabaseComponent;
};
