// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/GameState.h"
#include "FineGameState.generated.h"

class UFineSceneLoop;

UCLASS(Blueprintable, BlueprintType)
class FINEPLAY_API AFineGameState : public AGameState
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFineGameState();

	FORCEINLINE UFineSceneLoop* GetSceneLoop() const { return SceneLoop; }
	
private:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSoftClassPtr<UFineSceneLoop> SceneLoopClass;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFineSceneLoop> SceneLoop;
};
