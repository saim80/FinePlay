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

private:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UFineUserDatabaseComponent* UserDatabaseComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UFineSaveGameComponent* SaveGameComponent;
};
