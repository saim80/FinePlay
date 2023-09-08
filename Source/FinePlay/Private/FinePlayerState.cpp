// (c) 2023 Pururum LLC. All rights reserved.


#include "FinePlayerState.h"

#include "FineSaveGameComponent.h"
#include "Data/FineUserDatabaseComponent.h"

AFinePlayerState::AFinePlayerState(): Super()
{
	UserDatabaseComponent = CreateDefaultSubobject<UFineUserDatabaseComponent>(TEXT("UserDatabaseComponent"));
	SaveGameComponent = CreateDefaultSubobject<UFineSaveGameComponent>(TEXT("SaveGameComponent"));
}
