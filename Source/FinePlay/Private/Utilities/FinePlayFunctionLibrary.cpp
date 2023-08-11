// (c) 2023 Pururum LLC. All rights reserved.


#include "Utilities/FinePlayFunctionLibrary.h"

#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Scene/FineSceneLoop.h"

UFineSceneLoop* UFinePlayFunctionLibrary::GetSceneLoop(const UObject* WorldContextObject)
{
	const auto GameState = UGameplayStatics::GetGameState(WorldContextObject);
	if (GameState == nullptr)
	{
		return nullptr;
	}
	// Get component by class, UFineSceneLoop, from GameState.
	const auto Component = GameState->GetComponentByClass(UFineSceneLoop::StaticClass());
	// Cast the component to return type and return it.
	return Cast<UFineSceneLoop>(Component);
}
