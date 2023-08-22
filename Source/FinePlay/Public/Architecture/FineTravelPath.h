#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "FineTravelPath.generated.h"

class USphereComponent;

UENUM()
enum class EFineTravelPathMode : uint8
{
	Teleport,
	Navigation,
};

USTRUCT(BlueprintType)
struct FINEPLAY_API FFineTravelSchedule
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Travel")
	TWeakObjectPtr<AActor> Traveler;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Travel")
	TArray<USphereComponent*> TravelPoints;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Travel")
	int TravelIndex = 0;

	USphereComponent* GetCurrentTravelPoint()
	{
		if (TravelPoints.IsValidIndex(TravelIndex))
		{
			return TravelPoints[TravelIndex];
		}
		return nullptr;
	}

	bool IsReachedCurrentTravelPoint()
	{
		const auto Actor = Traveler.Get();
		if (!IsValid(Actor))
		{
			return false;
		}
		USphereComponent* CurrentTravelPoint = GetCurrentTravelPoint();
		if (!CurrentTravelPoint)
		{
			return false;
		}
		const auto Location = Actor->GetActorLocation();
		const auto TargetLocation = CurrentTravelPoint->GetComponentLocation();
		const auto Distance = FVector::Distance(Location, TargetLocation);
		return Distance < 5.f;
	}

	bool IsFinished() const
	{
		return TravelIndex >= TravelPoints.Num();
	}
};

class USphereComponent;
/// This class offers a character to travel between two points. These points are marked by sphere collision component.
///
/// For player characters, the characters enter one of the points and press the action button to travel to the other
///		point.
/// For AI characters, the characters enter one of the points and automatically travel to the other point.
/// For companion characters that follow player characters, the characters automatically travel between the two points
///		by following player characters.
///
/// The travel path is defined by two points. The points are marked by sphere collision component.
///
/// There are three travel modes:
/// - Teleport: The character is teleported from one point to the other point.
/// - Navigation: Using navigation mesh, the character is moved from one point to the other point.
///
/// When the characters are traveling, no user input is accepted. The characters become invincible.
///
/// In addition, one additional sphere collision component per point can be added to define the entrance.
/// When the characters enter the entrance, the characters are moved to the nearest point to move to the farthest point.
/// After completing the travel, the characters are moved to the closest entrance.
///
/// The movement between the travel point and entrance is always done by Controller mode.
///
/// After the trip, the character is released from invincibility and input is accepted for player characters.
UCLASS()
class FINEPLAY_API AFineTravelPath : public AActor
{
	GENERATED_BODY()

public:
	AFineTravelPath();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Travel")
	bool IsInUse() const { return CapturedCharacters.Num() > 0; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Travel")
	bool CanAcceptTravelers() const { return CapturedCharacters.Num() < Capacity; }

protected:
	void CaptureActor(AActor* Actor);
	void ReleaseActor(AActor* Actor);
	void HandleEntranceOverlap(AActor* Actor, UPrimitiveComponent* PrimitiveComponent);
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex);
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Travel")
	void OnTravelStarted(AActor* Actor);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Travel")
	void OnTravelEnded(AActor* Actor);

	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Travel", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> PointA;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Travel", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> PointB;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Travel", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> EntranceA;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Travel", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> EntranceB;

	UPROPERTY()
	TSet<AActor*> CapturedCharacters;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Travel", meta = (AllowPrivateAccess = "true"))
	EFineTravelPathMode Mode = EFineTravelPathMode::Teleport;

	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, FFineTravelSchedule> TravelSchedules;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Travel", meta = (AllowPrivateAccess = "true"))
	int32 Capacity = 1;
};
