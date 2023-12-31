﻿// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FineSceneLoop.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSceneWillLoad, const FString&, SceneName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSceneDidLoad, const FString&, SceneName);

class AFineScene;
/**
 * This class maintains scene actors by spawning and destroying them. This component is meant to be a part of
 * game state actor.
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = FinePlay, meta = (BlueprintSpawnableComponent))
class FINEPLAY_API UFineSceneLoop : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void AddScene(TSubclassOf<AFineScene> SceneClass);

	UFUNCTION(BlueprintCallable)
	void InsertScene(int32 NewIndex, TSubclassOf<AFineScene> SceneClass);

	UFUNCTION(BlueprintCallable)
	void PopScene();

	UFUNCTION(BlueprintCallable)
	void PlayNext();

	UFUNCTION(BlueprintCallable)
	void Clear();

	FORCEINLINE AFineScene* GetCurrentScene() const { return CurrentScene; }
	FORCEINLINE TArray<TSubclassOf<AFineScene>> GetSceneClasses() const { return SceneClasses; }

	static UFineSceneLoop* Get(UObject* WorldContext);

	UPROPERTY(BlueprintAssignable)
	FOnSceneWillLoad OnSceneWillLoad;
	UPROPERTY(BlueprintAssignable)
	FOnSceneDidLoad OnSceneDidLoad;

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AFineScene>> SceneClasses;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AFineScene* CurrentScene;

	void DestroyCurrentScene();
};
