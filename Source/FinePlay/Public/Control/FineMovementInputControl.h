// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FineCommonInputControl.h"
#include "InputAction.h"
#include "Components/ActorComponent.h"
#include "FineMovementInputControl.generated.h"

class UFineCharacterGameplay;
class UAbilitySystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCursorEffectSpawned, const FVector&, Location);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMovementStarted);

class UInputAction;
class UInputMappingContext;
class UNiagaraSystem;
/// Provides user input handling for character movement.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FINEPLAY_API UFineMovementInputControl : public UFineCommonInputControl
{
	GENERATED_BODY()

public:
	UFineMovementInputControl();

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	virtual void SetupInputComponent() override;
	FORCEINLINE const int32& GetRunActionInputID() const { return RunActionInputID; }

	UPROPERTY(BlueprintAssignable)
	FOnCursorEffectSpawned OnCursorEffectSpawned;

	UPROPERTY(BlueprintAssignable)
	FOnMovementStarted OnMovementStarted;

	FORCEINLINE void SetRunActionInputID(const int32& InRunActionID)
	{
		RunActionInputID = InRunActionID;
	}

	FORCEINLINE void SetJumpActionInputID(const int32& InJumpActionID)
	{
		JumpActionInputID = InJumpActionID;
	}

	FORCEINLINE const FVector& GetCachedDestination() const { return CachedDestination; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	virtual void OnInputStarted() override;
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnTouchTriggered();
	void OnTouchReleased();
	void OnWalkTriggered(const FInputActionInstance& InputActionInstance);
	void OnWalkReleased(const FInputActionInstance& InputActionInstance);
	void OnRunKeyTriggered();
	void OnRunKeyReleased();
	void OnRunTouchTriggered();
	void OnRunTouchReleased();
	void OnJumpTriggered();
	void OnJumpReleased();

	virtual void SpawnCursorEffect(const FVector& Location);

private:
	/// Used to remember the destination of the last movement input.
	FVector CachedDestination;
	/// Used to remember the time of the last movement input.
	float FollowTime;
	/// User's using touch interface?
	bool bIsTouch;

	/** Click Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationClickAction;

	/** Touch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationTouchAction;

	/** Keyboard Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* WalkAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* RunKeyAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* RunTouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	int32 RunActionInputID = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	int32 JumpActionInputID = INDEX_NONE;

	FTimerHandle RunDisableTimerHandle;

	// --------------------
	// Movement
	// --------------------
	FTimerHandle MovementTimerHandle;
	void UpdateAddMovementInput();

	// --------------------
	// Utility
	// --------------------
	bool IsCharacterRunning();
	bool IsCharacterJumping();
};
