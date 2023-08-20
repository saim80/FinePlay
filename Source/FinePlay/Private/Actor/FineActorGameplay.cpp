// (c) 2023 Pururum LLC. All rights reserved.


#include "Actor/FineActorGameplay.h"

#include "Data/FineDatabaseRecord.h"
#include "Data/FineLocalDatabaseComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

void UFineActorGameplay::BeginPlay()
{
	Super::BeginPlay();

	// Get game state
	const auto GameState = UGameplayStatics::GetGameState(this);
	// Get local database component
	LocalDatabaseComponent = GameState->FindComponentByClass<UFineLocalDatabaseComponent>();
	if (ensure(IsValid(LocalDatabaseComponent)))
	{
		// database record from local database component
		bool bSuccess = false;
		const auto Record = LocalDatabaseComponent->GetRecordByName(TEXT("DisplayData"), ActorName, bSuccess);
		if (bSuccess)
		{
			DisplayData.UpdateFromRecord(Record);
		}
	}
}

void UFineActorGameplay::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	LocalDatabaseComponent = nullptr;

	Super::EndPlay(EndPlayReason);
}
