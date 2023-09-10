// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "UObject/Object.h"
#include "FinePlayerState.generated.h"

class UFineSaveGameComponent;
class UFineUserDatabaseComponent;
/**
 * 
 */
UCLASS()
class FINEPLAY_API AFinePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AFinePlayerState();

	FORCEINLINE UFineUserDatabaseComponent* GetUserDatabaseComponent() const { return UserDatabaseComponent; }
	FORCEINLINE UFineUserDatabaseComponent* GetUserDatabaseComponent() { return UserDatabaseComponent; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnSaveGameLoaded();

private:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UFineUserDatabaseComponent* UserDatabaseComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UFineSaveGameComponent* SaveGameComponent;
};
