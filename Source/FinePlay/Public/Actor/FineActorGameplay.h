// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Data/FineDisplayData.h"
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
	FORCEINLINE const FFineDisplayData& GetDisplayData() const { return DisplayData; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	FORCEINLINE UFineLocalDatabaseComponent* GetLocalDatabaseComponent() const { return LocalDatabaseComponent; }

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineActorGameplay")
	FName ActorName;

private:

	UPROPERTY(BlueprintReadOnly, Category = "FineActorGameplay", meta = (AllowPrivateAccess = "true"))
	FFineDisplayData DisplayData;

	UPROPERTY()
	TObjectPtr<UFineLocalDatabaseComponent> LocalDatabaseComponent;
};
