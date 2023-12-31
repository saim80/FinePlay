﻿// (c) 2023 Pururum LLC. All rights reserved.


#include "Utilities/FinePlayFunctionLibrary.h"

#include "FineCountedFlag.h"
#include "FinePlayLog.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Scene/FineSceneLoop.h"
#include "WorldPartition/WorldPartition.h"

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

bool UFinePlayFunctionLibrary::IsStreamingCompleted(const UObject* WorldContextObject)
{
	// Get player controller.
	const auto PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	// Get streaming source from the player controller.
	TArray<FWorldPartitionStreamingSource> StreamingSources;
	if (PlayerController->GetStreamingSources(StreamingSources))
	{
		// Get world partition subsystem.
		const auto WorldPartition = WorldContextObject->GetWorld()->GetWorldPartition();
		if (IsValid(WorldPartition))
		{
			// Check if player controller's streaming sources are completed or not.
			return WorldPartition->IsStreamingCompleted(&StreamingSources);
		}
	}
	return false;
}

bool UFinePlayFunctionLibrary::IsStreamingNeeded(const UObject* WorldContextObject)
{
	// Get player controller.
	const auto PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	// Try to get world partition.
	const auto WorldPartition = WorldContextObject->GetWorld()->GetWorldPartition();
	if (IsValid(WorldPartition) == false)
	{
		return false;
	}
	return PlayerController->IsStreamingSourceEnabled() && IsStreamingCompleted(WorldContextObject) == false;
}

void UFinePlayFunctionLibrary::SetUserInputEnabled(AActor* Actor, const bool bEnabled, const FString Context)
{
	const auto Character = Cast<ACharacter>(Actor);
	if (IsValid(Character))
	{
		const auto Controller = Character->GetController();
		if (IsValid(Controller))
		{
			const auto Flag = Controller->FindComponentByClass<UFineCountedFlag>();
			Flag->SetEnabled(bEnabled);
			FP_LOG("Flag change attempted: %s, actual value %s: %s", bEnabled ? TEXT("enabled") : TEXT("disabled"),
			       Flag->IsEnabled() ? TEXT("enabled") : TEXT("disabled"), *Context);
		}
	}
}

bool UFinePlayFunctionLibrary::GetUserInputEnabled(AActor* Actor)
{
	const auto Character = Cast<ACharacter>(Actor);
	if (IsValid(Character))
	{
		const auto Controller = Character->GetController();
		if (IsValid(Controller))
		{
			const auto Flag = Controller->FindComponentByClass<UFineCountedFlag>();
			return Flag->IsEnabled();
		}
	}
	return false;
}
