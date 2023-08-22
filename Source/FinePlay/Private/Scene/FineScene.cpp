// (c) 2023 Pururum LLC. All rights reserved.


#include "Scene/FineScene.h"

#include "FineCountedFlag.h"
#include "FinePlayLog.h"
#include "FineSaveGameComponent.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Utilities/FinePlayFunctionLibrary.h"
#include "GameFramework/SpectatorPawn.h"

// Sets default values
AFineScene::AFineScene(): Super()
{
	LoadingScreenCountedFlag = CreateDefaultSubobject<UFineCountedFlag>(TEXT("LoadingScreenCountedFlag"));
	LoadingScreenCountedFlag->SetFlagName(TEXT("LoadingScreen"));
	// Lifecycle is controlled by UFineSceneLoop, not world partition.
	SetIsSpatiallyLoaded(false);
}

void AFineScene::BeginPlay()
{
	Super::BeginPlay();
	LoadingScreenCountedFlag->OnFlagUpdated.AddDynamic(this, &AFineScene::UpdateLoadingScreenVisibility);

	if (NeedsToLoadGameData())
	{
		LoadGameData();
	}
	if (NeedsToLoadPlayerData())
	{
		LoadPlayerData();
	}
	TryTeleportToScene();
}

void AFineScene::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	LoadingScreenCountedFlag->OnFlagUpdated.RemoveDynamic(this, &AFineScene::UpdateLoadingScreenVisibility);
	Super::EndPlay(EndPlayReason);
}

void AFineScene::UpdateLoadingScreenVisibility(UFineCountedFlag* Flag, const bool bNewFlag)
{
	if (bNewFlag)
	{
		OnLoadingStarted();
	}
	else
	{
		OnLoadingFinished();
	}
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
	if (NeedsToLoadGameData() || NeedsToLoadPlayerData())
	{
		FP_LOG("Cannot teleport to the scene's player start yet. game data or player data is not loaded.");
	}
	// Check player controller if the controlled pawn is spectator.
	// If so, spawn player pawn according to the game mode.
	// And, possess the pawn.
	const auto PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	const auto Pawn = PlayerController->GetPawn();
	if (!IsValid(Pawn) || Pawn->IsA<ASpectatorPawn>())
	{
		const auto GameMode = UGameplayStatics::GetGameMode(this);
		const auto PlayerStart = GameMode->FindPlayerStart(PlayerController, PlayerStartTag);
		const auto PlayerPawn = GameMode->SpawnDefaultPawnFor(PlayerController, PlayerStart);
		PlayerController->Possess(PlayerPawn);

		GameMode->RestartPlayer(PlayerController);
	}
	/// Get game mode
	const auto GameMode = UGameplayStatics::GetGameMode(this);
	/// Find player start with the tag.
	const AActor* PlayerStart = GameMode->FindPlayerStart(PlayerController, PlayerStartTag);
	/// Get pawn for the controller.
	/// If the pawn is valid, teleport it to the player start.
	if (const auto PlayerPawn = PlayerController->GetPawn())
	{
		const auto SourceLocation = PlayerPawn->GetActorLocation();
		const auto TargetLocation = PlayerStart->GetActorLocation();
		/// If the pawn is nearly not at the player start, teleport it.
		if ((TargetLocation - SourceLocation).IsNearlyZero(0.1f))
		{
			PlayerPawn->TeleportTo(TargetLocation, PlayerStart->GetActorRotation());
		}
		if (UFinePlayFunctionLibrary::IsStreamingNeeded(this))
		{
			LoadingScreenCountedFlag->SetEnabled(true);
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
		LoadingScreenCountedFlag->SetEnabled(false);
	}
}

bool AFineScene::IsPlayerDataLoaded() const
{
	const auto PlayerState = UGameplayStatics::GetPlayerState(this, 0);
	if (PlayerState == nullptr)
	{
		return false;
	}
	const auto PlayerData = PlayerState->FindComponentByClass<UFineSaveGameComponent>();
	if (PlayerData == nullptr)
	{
		return false;
	}
	return PlayerData->IsLoaded();
}

bool AFineScene::IsGameDataLoaded() const
{
	const auto GameState = UGameplayStatics::GetGameState(this);
	if (GameState == nullptr)
	{
		return false;
	}
	const auto GameData = GameState->FindComponentByClass<UFineSaveGameComponent>();
	if (GameData == nullptr)
	{
		return false;
	}
	return GameData->IsLoaded();
}

void AFineScene::OnGameDataLoaded()
{
	check(IsGameDataLoaded());

	const auto GameState = UGameplayStatics::GetGameState(this);
	if (GameState == nullptr)
	{
		FP_LOG("GameState is null.");
		return;
	}
	const auto GameData = GameState->FindComponentByClass<UFineSaveGameComponent>();
	if (GameData == nullptr)
	{
		FP_LOG("GameData is null.");
		return;
	}
	GameData->OnSaveGameLoaded.RemoveDynamic(this, &AFineScene::OnGameDataLoaded);

	if (IsPlayerDataLoaded())
	{
		TryTeleportToScene();
	}
	LoadingScreenCountedFlag->SetEnabled(false);
}

void AFineScene::OnPlayerDataLoaded()
{
	check(IsPlayerDataLoaded());

	const auto PlayerState = UGameplayStatics::GetPlayerState(this, 0);
	if (PlayerState == nullptr)
	{
		FP_LOG("Player state is null.");
		return;
	}
	const auto PlayerData = PlayerState->FindComponentByClass<UFineSaveGameComponent>();
	if (PlayerData == nullptr)
	{
		FP_LOG("Player data is null.");
		return;
	}
	PlayerData->OnSaveGameLoaded.RemoveDynamic(this, &AFineScene::OnPlayerDataLoaded);

	if (IsGameDataLoaded())
	{
		TryTeleportToScene();
	}
	LoadingScreenCountedFlag->SetEnabled(false);
}

void AFineScene::LoadGameData()
{
	if (IsGameDataLoaded())
	{
		return;
	}
	if (const auto GameState = UGameplayStatics::GetGameState(this))
	{
		if (const auto GameData = GameState->FindComponentByClass<UFineSaveGameComponent>())
		{
			GameData->OnSaveGameLoaded.AddDynamic(this, &AFineScene::OnGameDataLoaded);
			GameData->AsyncLoadProgress();
			LoadingScreenCountedFlag->SetEnabled(true);
		}
	}
}

void AFineScene::LoadPlayerData()
{
	if (IsPlayerDataLoaded())
	{
		return;
	}
	if (const auto PlayerState = UGameplayStatics::GetPlayerState(this, 0))
	{
		if (const auto PlayerData = PlayerState->FindComponentByClass<UFineSaveGameComponent>())
		{
			PlayerData->OnSaveGameLoaded.AddDynamic(this, &AFineScene::OnPlayerDataLoaded);
			PlayerData->AsyncLoadProgress();
			LoadingScreenCountedFlag->SetEnabled(true);
		}
	}
}
