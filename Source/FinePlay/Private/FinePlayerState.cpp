// (c) 2023 Pururum LLC. All rights reserved.


#include "FinePlayerState.h"

#include "FineSaveGameComponent.h"
#include "Data/FineUserDatabaseComponent.h"

AFinePlayerState::AFinePlayerState(): Super()
{
	UserDatabaseComponent = CreateDefaultSubobject<UFineUserDatabaseComponent>(TEXT("UserDatabaseComponent"));
	SaveGameComponent = CreateDefaultSubobject<UFineSaveGameComponent>(TEXT("SaveGameComponent"));
}

void AFinePlayerState::BeginPlay()
{
	Super::BeginPlay();
	SaveGameComponent->OnSaveGameLoaded.AddDynamic(this, &AFinePlayerState::OnSaveGameLoaded);
}

void AFinePlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SaveGameComponent->OnSaveGameLoaded.RemoveDynamic(this, &AFinePlayerState::OnSaveGameLoaded);
	Super::EndPlay(EndPlayReason);
}

void AFinePlayerState::OnSaveGameLoaded()
{
	check(SaveGameComponent->IsLoaded());
	const auto DBPath = FString::Printf(TEXT("%s-%d.db"), *SaveGameComponent->GetSlot(),
	                                    SaveGameComponent->GetUserIndex());
	UserDatabaseComponent->SetDatabasePath(DBPath);
}
