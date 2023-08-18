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
		const auto Record = LocalDatabaseComponent->GetRecordByName(EntityName, ActorName, bSuccess);
		if (bSuccess)
		{
			ActorData.UpdateFromRecord(Record);
			Health = ActorData.Health;
		}
		if (Health > 0.0f)
		{
			const FGameplayTag NewTag = FGameplayTag::RequestGameplayTag(TEXT("Actor.State.Alive"));
			GameplayTags.AddTag(NewTag);
		}
	}
}

void UFineActorGameplay::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	LocalDatabaseComponent = nullptr;

	Super::EndPlay(EndPlayReason);
}

void UFineActorGameplay::SetHealth(int32 NewHealth)
{
	// Update NewHealth by Clamping the value between 0 and MaxHealth
	NewHealth = FMath::Clamp(NewHealth, 0, ActorData.Health);
	// return if health is not changed
	if (Health == NewHealth)
	{
		return;
	}
	// Update Health
	const int32 OldHealth = Health;
	Health = NewHealth;
	// if NewHealth is 0, remove Alive tag.
	if (NewHealth == 0)
	{
		const FGameplayTag AliveTag = FGameplayTag::RequestGameplayTag(TEXT("Actor.State.Alive"));
		GameplayTags.RemoveTag(AliveTag);
	}
	// Broadcast OnHealthUpdated
	OnHealthUpdated.Broadcast(GetOwner(), NewHealth, OldHealth);
}
