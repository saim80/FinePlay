// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FineScene.generated.h"

class UFineSceneLoop;

/**
 * This class prepares and clean up for a scene. A scene is a unit of a gameplay in open world games.
 *
 * A scene actor is not supposed to be spawned by other means nor streamed in/out.
 *
 * A scene actor does not have any presentation in the world.
 */
UCLASS(Blueprintable, BlueprintType)
class FINEPLAY_API AFineScene : public AInfo
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFineScene();

private:
	friend class UFineSceneLoop;
	TWeakObjectPtr<UFineSceneLoop> SceneLoop;
};
