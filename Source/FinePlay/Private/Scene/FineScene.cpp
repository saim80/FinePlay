// (c) 2023 Pururum LLC. All rights reserved.


#include "Scene/FineScene.h"

#include "FineCountedFlag.h"
#include "FineGameState.h"
#include "FinePlayLog.h"
#include "FineSaveGameComponent.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Utilities/FinePlayFunctionLibrary.h"
#include "Scene/FineSceneLoop.h"

// Sets default values
AFineScene::AFineScene(): Super()
{
	LoadingScreenCountedFlag = CreateDefaultSubobject<UFineCountedFlag>(TEXT("LoadingScreenCountedFlag"));
	LoadingScreenCountedFlag->SetFlagName(TEXT("LoadingScreen"));
	// Lifecycle is controlled by UFineSceneLoop, not world partition.
	SetIsSpatiallyLoaded(false);
	SetTickableWhenPaused(true);
}

AFineScene* AFineScene::GetCurrentScene(const UObject* WorldContextObject)
{
	const auto GameState = Cast<AFineGameState>(UGameplayStatics::GetGameState(WorldContextObject));
	if (!IsValid(GameState))
	{
		FP_LOG("Invalid game state.");
		return nullptr;
	}
	const auto SceneLoop = GameState->GetSceneLoop();
	if (!IsValid(SceneLoop))
	{
		FP_LOG("Invalid scene loop.");
		return nullptr;
	}
	return SceneLoop->GetCurrentScene();
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
	const auto PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (IsValid(PlayerPawn))
	{
		PlayerPawn->Destroy();
	}
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
		// Adding some delay to prevent flickering.
		GetWorldTimerManager().SetTimer(LoadingScreenTimerHandle, this, &AFineScene::OnLoadingFinished, 0.2f, false);
	}
}

void AFineScene::OnLoadingStarted_Implementation()
{
	FP_LOG("Loading started. %s", *GetPlayerStartTag());
	if (SceneLoop.IsValid())
	{
		SceneLoop->OnSceneWillLoad.Broadcast(PlayerStartTag);
	}
}

void AFineScene::OnLoadingFinished_Implementation()
{
	FP_LOG("Loading finished. %s", *GetPlayerStartTag());
	if (SceneLoop.IsValid())
	{
		SceneLoop->OnSceneDidLoad.Broadcast(PlayerStartTag);
	}
}

void AFineScene::TryTeleportToScene()
{
	if (NeedsToLoadGameData())
	{
		FP_LOG("Cannot teleport to the scene's player start yet. game data is not loaded.");
		return;
	}
	if (NeedsToLoadPlayerData())
	{
		FP_LOG("Cannot teleport to the scene's player start yet. player data is not loaded.");
		return;
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
	const auto PlayerStart = Cast<APlayerStart>(GameMode->FindPlayerStart(PlayerController, PlayerStartTag));
	if (PlayerStartTag != PlayerStart->PlayerStartTag.ToString())
	{
		FP_ERROR("Invalid player start found: expected %s, returned: %s", *PlayerStartTag,
		         *PlayerStart->PlayerStartTag.ToString());
		return;
	}
	/// Get pawn for the controller.
	/// If the pawn is valid, teleport it to the player start.
	if (const auto PlayerPawn = PlayerController->GetPawn())
	{
		const auto SourceLocation = PlayerPawn->GetActorLocation();
		const auto TargetLocation = PlayerStart->GetActorLocation();
		/// If the pawn is nearly not at the player start, teleport it.
		if (!FMath::IsNearlyZero((TargetLocation - SourceLocation).Length()))
		{
			PlayerPawn->TeleportTo(TargetLocation, PlayerStart->GetActorRotation());
			FP_LOG("Teleporting player pawn to: %s", *PlayerStartTag);
		}
		else
		{
			FP_LOG("Too close. Won't teleport the player pawn: %s", *PlayerStartTag);
		}
		LoadingScreenCountedFlag->SetEnabled(true);
		if (UFinePlayFunctionLibrary::IsStreamingNeeded(this))
		{
			// Start timer with 0.1 second delay.
			GetWorldTimerManager().SetTimer(LoadingScreenTimerHandle, this, &AFineScene::TryHideLoadingScreen, 0.1f,
			                                true);
		}
		else
		{
			TryHideLoadingScreen();
		}
	}
	else
	{
		FP_LOG("Player pawn is null.");
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

	FP_LOG("Game data loaded.");
	TryTeleportToScene();
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

	FP_LOG("Player data loaded.");
	TryTeleportToScene();
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
			FP_LOG("Game data loading started.");
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
			FP_LOG("Player data loading started.");
		}
	}
}
