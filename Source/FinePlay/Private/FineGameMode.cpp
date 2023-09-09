// (c) 2023 Pururum LLC. All rights reserved.


#include "FineGameMode.h"

#include "FinePlayLog.h"
#include "Scene/FineScene.h"

AActor* AFineGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	/// Get actor of AFineScene class.
	const auto Scene = AFineScene::GetCurrentScene(this);
	/// if the scene is valid, return the player start actor with the tag.
	if (IsValid(Scene))
	{
		return Super::FindPlayerStart(Player, Scene->GetPlayerStartTag());
	}
	return Super::ChoosePlayerStart_Implementation(Player);
}

bool AFineGameMode::PlayerCanRestart_Implementation(APlayerController* Player)
{
	const auto OldResult = Super::PlayerCanRestart_Implementation(Player);
	/// Get actor of AFineScene class.
	const auto Scene = AFineScene::GetCurrentScene(this);
	if (IsValid(Scene))
	{
		/// If scene's tag is not empty, return true.
		const auto NewResult = Scene->GetPlayerStartTag().IsEmpty() == false && !Scene->NeedsToLoadGameData() &&
			!Scene->NeedsToLoadPlayerData();
		/// Returning false will cause the default pawn to be spectator.
		return OldResult && NewResult;
	}
	return OldResult;
}

UClass* AFineGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	const auto CurrentScene = AFineScene::GetCurrentScene(this);
	UClass* OutClass = nullptr;
	if (IsValid(CurrentScene))
	{
		OutClass = CurrentScene->GetDefaultPawnClass();
		if (IsValid(OutClass))
		{
			FP_LOG("Picking default pawn class for controller: %s", *OutClass->GetName());
		}
	}
	if (!IsValid(OutClass))
	{
		OutClass = Super::GetDefaultPawnClassForController_Implementation(InController);
		if (IsValid(OutClass))
		{
			FP_LOG("No pawn class found, falling back to default: %s", *OutClass->GetName());
		}
	}
	return OutClass;
}
