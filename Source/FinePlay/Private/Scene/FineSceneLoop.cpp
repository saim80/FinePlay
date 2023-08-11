// (c) 2023 Pururum LLC. All rights reserved.


#include "Scene/FineSceneLoop.h"

#include "Scene/FineScene.h"

void UFineSceneLoop::AddScene(TSubclassOf<AFineScene> SceneClass)
{
	// Add the new class to scene array
	SceneClasses.Add(SceneClass);
	// if scene classes length is 1, play the scene
	if (SceneClasses.Num() == 1)
	{
		PlayNext();
	}
	else
	{
		PopScene();
	}
}

void UFineSceneLoop::InsertScene(int32 NewIndex, TSubclassOf<AFineScene> SceneClass)
{
	// Insert the new class to scene array
	SceneClasses.Insert(SceneClass, NewIndex);
	// if scene classes length is 1, play the scene
	if (SceneClasses.Num() == 1)
	{
		PlayNext();
	}
	else
	{
		PopScene();
	}
}

void UFineSceneLoop::PopScene()
{
	// Remove the first class in the scene classes array.
	SceneClasses.RemoveAt(0);
	PlayNext();
}

void UFineSceneLoop::PlayNext()
{
	DestroyCurrentScene();
	// if scene classes are empty, return
	if (SceneClasses.Num() == 0)
	{
		return;
	}
	// Get the first class in the scene classes array.
	const TSubclassOf<AFineScene> SceneClass = SceneClasses[0];
	// Spawn the scene actor.
	AFineScene* Scene = GetWorld()->SpawnActor<AFineScene>(SceneClass);
	Scene->SceneLoop = this;
	// Assign the current scene.
	CurrentScene = Scene;
}

void UFineSceneLoop::Clear()
{
	DestroyCurrentScene();
	// Clear the scene classes array.
	SceneClasses.Empty();
}

void UFineSceneLoop::DestroyCurrentScene()
{
	// Destroy the current scene and clear the pointer.
	if (IsValid(CurrentScene))
	{
		CurrentScene->SceneLoop = nullptr;
		CurrentScene->Destroy();
		CurrentScene = nullptr;
	}
}
