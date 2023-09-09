// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameState.h"
#include "FineGameState.generated.h"

class UFineSaveGameComponent;
class UFineLocalDatabaseComponent;
class UFineSceneLoop;

UCLASS(Blueprintable, BlueprintType)
class FINEPLAY_API AFineGameState : public AGameState
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFineGameState();

	FORCEINLINE UFineSceneLoop* GetSceneLoop() const { return SceneLoop; }

	FORCEINLINE UFineLocalDatabaseComponent* GetLocalDatabaseComponent() const { return LocalDatabaseComponent; }
	FORCEINLINE UFineSaveGameComponent* GetSaveGameComponent() const { return SaveGameComponent; }

private:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFineSceneLoop> SceneLoop;

	/// Local data base to pull the game wide data from. This is read only database.
	/// Using local database is only useful for single player games.
	/// For multiplayer games, use a server database or replication.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFineLocalDatabaseComponent> LocalDatabaseComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UFineSaveGameComponent* SaveGameComponent;
};
