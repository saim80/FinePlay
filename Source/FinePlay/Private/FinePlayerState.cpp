// (c) 2023 Pururum LLC. All rights reserved.


#include "FinePlayerState.h"

#include "Data/FineUserDatabaseComponent.h"

AFinePlayerState::AFinePlayerState(): Super()
{
	UserDatabaseComponent = CreateDefaultSubobject<UFineUserDatabaseComponent>(TEXT("UserDatabaseComponent"));
}
