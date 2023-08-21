// (c) 2023 Pururum LLC. All rights reserved.


#include "Scene/FineScene.h"

#include "FinePlayLog.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Utilities/FinePlayFunctionLibrary.h"

// Sets default values
AFineScene::AFineScene(): Super()
{
	// Lifecycle is controlled by UFineSceneLoop, not world partition.
	SetIsSpatiallyLoaded(false);
}

void AFineScene::BeginPlay()
{
	Super::BeginPlay();
	TryTeleportToScene();
}

void AFineScene::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AFineScene::OnLoadingStarted_Implementation()
{
	FP_LOG("Loading started. %s", *GetPlayerStartTag());
}

void AFineScene::OnLoadingFinished_Implementation()
{
	FP_LOG("Loading finished. %s", *GetPlayerStartTag());
}

void AFineScene::TryTeleportToScene()
{
	/// Get game mode
	const auto GameMode = UGameplayStatics::GetGameMode(this);
	/// Get player controller.
	const auto PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	/// Find player start with the tag.
	const AActor* PlayerStart = GameMode->FindPlayerStart(PlayerController, PlayerStartTag);
	/// Get pawn for the controller.
	/// If the pawn is valid, teleport it to the player start.
	if (const auto Pawn = PlayerController->GetPawn())
	{
		const auto SourceLocation = Pawn->GetActorLocation();
		const auto TargetLocation = PlayerStart->GetActorLocation();
		/// If the pawn is nearly not at the player start, teleport it.
		if ((TargetLocation - SourceLocation).IsNearlyZero(0.1f))
		{
			Pawn->TeleportTo(TargetLocation, PlayerStart->GetActorRotation());
		}
		if (UFinePlayFunctionLibrary::IsStreamingNeeded(this))
		{
			OnLoadingStarted();
			// Start timer with 0.1 second delay.
			GetWorldTimerManager().SetTimer(LoadingScreenTimerHandle, this, &AFineScene::TryHideLoadingScreen, 0.1f,
			                                true);
		}
	}
}

void AFineScene::TryHideLoadingScreen()
{
	if (UFinePlayFunctionLibrary::IsStreamingCompleted(this))
	{
		// Clear timer.
		GetWorldTimerManager().ClearTimer(LoadingScreenTimerHandle);
		OnLoadingFinished();
	}
}
