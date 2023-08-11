// (c) 2023 Pururum LLC. All rights reserved.


#include "FineGameState.h"

#include "Scene/FineSceneLoop.h"


// Sets default values
AFineGameState::AFineGameState(): Super()
{
	TSubclassOf<UFineSceneLoop> TargetClass;
	if (SceneLoopClass.IsNull())
	{
		TargetClass = UFineSceneLoop::StaticClass();
	}
	else
	{
		TargetClass = SceneLoopClass.LoadSynchronous();
	}
	SceneLoop = Cast<UFineSceneLoop>(CreateDefaultSubobject(TEXT("SceneLoop"), TargetClass, TargetClass, true, false));
}
