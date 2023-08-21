// (c) 2023 Pururum LLC. All rights reserved.


#include "Actor/FinePaperCharacter.h"

#include "AbilitySystemComponent.h"
#include "Actor/FineCharacterGameplay.h"


// Sets default values
AFinePaperCharacter::AFinePaperCharacter(): Super()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
}

void AFinePaperCharacter::BeginPlay()
{
	Super::BeginPlay();
	// Initialize on the character right away. This is needed for the ability system to work.
	//
	// Different replication modes will require different initialization setup.
	// The current implementation assumes Full replication for single player games.
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}
