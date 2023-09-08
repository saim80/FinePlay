// (c) 2023 Pururum LLC. All rights reserved.


#include "FineGameState.h"

#include "FineSaveGameComponent.h"
#include "Scene/FineSceneLoop.h"
#include "Data/FineLocalDatabaseComponent.h"


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
	LocalDatabaseComponent = CreateDefaultSubobject<UFineLocalDatabaseComponent>(TEXT("LocalDatabaseComponent"));
	SaveGameComponent = CreateDefaultSubobject<UFineSaveGameComponent>(TEXT("SaveGameComponent"));
}
