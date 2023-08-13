﻿// (c) 2023 Pururum LLC. All rights reserved.


#include "FineGameMode.h"

#include "Scene/FineScene.h"

AActor* AFineGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	/// Get actor of AFineScene class.
	const auto Scene = Cast<AFineScene>(Player->GetPawn()->GetOwner());
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
	const auto Scene = Cast<AFineScene>(Player->GetPawn()->GetOwner());
	if (IsValid(Scene))
	{
		/// If scene's tag is not empty, return true.
		const auto NewResult = Scene->GetPlayerStartTag().IsEmpty() == false;
		return OldResult && NewResult;
	}
	return OldResult;
}