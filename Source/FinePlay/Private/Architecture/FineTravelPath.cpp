#include "Architecture/FineTravelPath.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "FinePlayLog.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AFineTravelPath::AFineTravelPath(): Super()
{
	// create all sphere components and make them overlap with pawn.
	PointA = CreateDefaultSubobject<USphereComponent>(TEXT("PointA"));
	PointB = CreateDefaultSubobject<USphereComponent>(TEXT("PointB"));
	EntranceA = CreateDefaultSubobject<USphereComponent>(TEXT("EntranceA"));
	EntranceB = CreateDefaultSubobject<USphereComponent>(TEXT("EntranceB"));

	const TArray<USphereComponent*> SphereComponents = {PointA, PointB, EntranceA, EntranceB};
	for (const auto Sphere : SphereComponents)
	{
		Sphere->SetCollisionProfileName(TEXT("Trigger"));
		Sphere->SetSphereRadius(10.f);
		Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		Sphere->SetupAttachment(GetRootComponent());
	}
}

void AFineTravelPath::HandleTravelPointOverlap(AActor* Actor, UPrimitiveComponent* PrimitiveComponent)
{
	if (!IsInUse())
	{
		return;
	}
	if (!CapturedCharacters.Contains(Actor))
	{
		return;
	}
}

void AFineTravelPath::CaptureActor(AActor* Actor)
{
	CapturedCharacters.Add(Actor);
	// Disable the user input.
	const auto PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	Actor->DisableInput(PlayerController);

	// Make the character invincible.
	const auto AbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
	if (IsValid(AbilitySystem))
	{
		AbilitySystem->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Actor.State.Invincible")));
	}
	// Disable gravity of the character movement component to enable vertical movement.
	const auto CharacterMovement = Actor->FindComponentByClass<UCharacterMovementComponent>();
	if (IsValid(CharacterMovement))
	{
		CharacterMovement->GravityScale = 0.f;
	}
}

void AFineTravelPath::ReleaseActor(AActor* Actor)
{
	// Enable gravity of the character movement component to disable vertical movement.
	const auto CharacterMovement = Actor->FindComponentByClass<UCharacterMovementComponent>();
	if (IsValid(CharacterMovement))
	{
		CharacterMovement->GravityScale = 1.f;
	}
	// Make the character not invincible.
	const auto AbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
	if (IsValid(AbilitySystem))
	{
		AbilitySystem->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Actor.State.Invincible")));
	}

	// Enable the user input.
	const auto PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	Actor->EnableInput(PlayerController);
	CapturedCharacters.Remove(Actor);
}

void AFineTravelPath::HandleEntranceOverlap(AActor* Actor, UPrimitiveComponent* PrimitiveComponent)
{
	if (CanAcceptTravelers())
	{
		FP_LOG("%s is entering %s", *Actor->GetName(), *PrimitiveComponent->GetName());
		TArray<USphereComponent*> TravelPoints;
		if (PrimitiveComponent == EntranceA)
		{
			TravelPoints.Add(PointA);
			TravelPoints.Add(PointB);
			TravelPoints.Add(EntranceB);
		}
		else
		{
			TravelPoints.Add(PointB);
			TravelPoints.Add(PointA);
			TravelPoints.Add(EntranceA);
		}
		FFineTravelSchedule NewSchedule;
		NewSchedule.Traveler = Actor;
		NewSchedule.TravelPoints = TravelPoints;
		NewSchedule.TravelIndex = 0;
		CaptureActor(Actor);
		OnTravelStarted(Actor);
		TravelSchedules.Add(Actor, NewSchedule);
	}
}

void AFineTravelPath::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                     const FHitResult& SweepResult)
{
	if (!OtherActor->IsA<ACharacter>())
	{
		FP_LOG("Ignored actor: %s", *OtherActor->GetName());
		return;
	}
	if (OverlappedComponent == PointA || OverlappedComponent == PointB)
	{
		HandleTravelPointOverlap(OtherActor, OverlappedComponent);
	}
	else if (OverlappedComponent == EntranceA || OverlappedComponent == EntranceB)
	{
		HandleEntranceOverlap(OtherActor, OverlappedComponent);
	}
}

void AFineTravelPath::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor->IsA<ACharacter>())
	{
		FP_LOG("Ignored actor: %s", *OtherActor->GetName());
		return;
	}
	if (CapturedCharacters.Contains(OtherActor))
	{
		FP_LOG("%s is leaving %s", *OtherActor->GetName(), *OverlappedComponent->GetName());
	}
}

void AFineTravelPath::BeginPlay()
{
	Super::BeginPlay();

	// For all sphere components, bind overlap events.
	const TArray<USphereComponent*> SphereComponents = {PointA, PointB, EntranceA, EntranceB};
	for (const auto Sphere : SphereComponents)
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &AFineTravelPath::OnOverlapBegin);
		Sphere->OnComponentEndOverlap.AddDynamic(this, &AFineTravelPath::OnOverlapEnd);
	}
}

void AFineTravelPath::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// For all sphere components, unbind overlap events.
	const TArray<USphereComponent*> SphereComponents = {PointA, PointB, EntranceA, EntranceB};
	for (const auto Sphere : SphereComponents)
	{
		Sphere->OnComponentBeginOverlap.RemoveDynamic(this, &AFineTravelPath::OnOverlapBegin);
		Sphere->OnComponentEndOverlap.RemoveDynamic(this, &AFineTravelPath::OnOverlapEnd);
	}
	Super::EndPlay(EndPlayReason);
}

void AFineTravelPath::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// For all travel schedules, remove invalid travel schedules.
	TravelSchedules.Remove(nullptr);

	// For all travel schedules, remove the traveling schedule if the travel is finished.
	TArray<FFineTravelSchedule> FinishedSchedules;
	TravelSchedules.GenerateValueArray(FinishedSchedules);
	for (auto Schedule : FinishedSchedules)
	{
		if (Schedule.IsFinished())
		{
			const auto Actor = Schedule.Traveler.Get();
			ReleaseActor(Actor);
			OnTravelEnded(Actor);
			TravelSchedules.Remove(Actor);
		}
	}

	// For all travel schedules, move the traveler to the next travel point.
	for (auto& Schedule : TravelSchedules)
	{
		const auto Traveler = Schedule.Value.Traveler.Get();
		const auto TravelPoints = Schedule.Value.TravelPoints;
		const auto TravelIndex = Schedule.Value.TravelIndex;
		const auto TravelPoint = TravelPoints[TravelIndex];
		const auto TravelPointLocation = TravelPoint->GetComponentLocation();
		const auto TravelerLocation = Traveler->GetActorLocation();
		if (Schedule.Value.IsReachedCurrentTravelPoint())
		{
			Schedule.Value.TravelIndex++;
		}
		else
		{
			if (Mode == EFineTravelPathMode::Navigation || TravelIndex == 0 || TravelIndex == TravelPoints.Num() - 1)
			{
				const auto TravelerToTravelPoint = TravelPointLocation - TravelerLocation;
				const auto TravelerToTravelPointDirection = TravelerToTravelPoint.GetSafeNormal();
				const auto TravelerMovement = Traveler->FindComponentByClass<UCharacterMovementComponent>();
				if (IsValid(TravelerMovement))
				{
					TravelerMovement->AddInputVector(TravelerToTravelPointDirection);
				}
			}
			else
			{
				Traveler->SetActorLocation(TravelPointLocation);
			}
		}
	}
}

void AFineTravelPath::OnTravelEnded_Implementation(AActor* Actor)
{
	FP_LOG("Travel started for %s", *Actor->GetName());
}

void AFineTravelPath::OnTravelStarted_Implementation(AActor* Actor)
{
	FP_LOG("Travel ended for %s", *Actor->GetName());
}
