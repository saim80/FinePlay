// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FineScene.generated.h"

class UFineWidgetScreen;
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
	/// Sets default values for this actor's properties
	AFineScene();

	/// Player start tag to use for this scene.
	FORCEINLINE const FString& GetPlayerStartTag() const { return PlayerStartTag; }

protected:
	/// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// Called when the game ends or when destroyed
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Scene", meta = (AllowPrivateAccess = true))
	UFineWidgetScreen* LoadingScreen;

private:
	friend class UFineSceneLoop;
	TWeakObjectPtr<UFineSceneLoop> SceneLoop;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Scene", meta = (AllowPrivateAccess = true))
	FString PlayerStartTag;


	/// Teleport the player pawn to the player start of this scene.
	void TryTeleportToScene();
	UFUNCTION(meta = (AllowPrivateAccess = true))
	void TryHideLoadingScreen();

	FTimerHandle LoadingScreenTimerHandle;
};
