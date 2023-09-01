// (c) 2023 Pururum LLC. All rights reserved.


#include "Control/FineMovementInputControl.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "FinePlayLog.h"
#include "NiagaraFunctionLibrary.h"
#include "Actor/FineCharacterGameplay.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values for this component's properties
UFineMovementInputControl::UFineMovementInputControl(): Super()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.0f;
	ShortPressThreshold = 0.3f;
}

void UFineMovementInputControl::SetupInputComponent()
{
	Super::SetupInputComponent();
	// Get owner as player controller
	const auto PlayerController = CastChecked<APlayerController>(GetOwner());

	// Get input component from player controller
	const auto InputComponent = PlayerController->InputComponent;
	if (!IsValid(InputComponent))
	{
		return;
	}
	if (!IsActive())
	{
		return;
	}
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		ActionBindings.Add(EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this,
		                                                      &UFineMovementInputControl::OnInputStarted));
		ActionBindings.Add(EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this,
		                                                      &UFineMovementInputControl::OnSetDestinationTriggered));
		ActionBindings.Add(EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this,
		                                                      &UFineMovementInputControl::OnSetDestinationReleased));
		ActionBindings.Add(EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this,
		                                                      &UFineMovementInputControl::OnSetDestinationReleased));

		// Setup touch input events
		ActionBindings.Add(EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this,
		                                                      &UFineMovementInputControl::OnInputStarted));
		ActionBindings.Add(EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this,
		                                                      &UFineMovementInputControl::OnTouchTriggered));
		ActionBindings.Add(EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this,
		                                                      &UFineMovementInputControl::OnTouchReleased));
		ActionBindings.Add(EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this,
		                                                      &UFineMovementInputControl::OnTouchReleased));

		// Setup Walk input events
		ActionBindings.Add(EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Started, this,
		                                                      &UFineMovementInputControl::OnInputStarted));
		ActionBindings.Add(EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Triggered, this,
		                                                      &UFineMovementInputControl::OnWalkTriggered));
		ActionBindings.Add(EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Completed, this,
		                                                      &UFineMovementInputControl::OnWalkReleased));
		ActionBindings.Add(EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Canceled, this,
		                                                      &UFineMovementInputControl::OnWalkReleased));

		// Setup Run input events
		ActionBindings.Add(EnhancedInputComponent->BindAction(RunKeyAction, ETriggerEvent::Started, this,
		                                                      &UFineMovementInputControl::OnInputStarted));
		ActionBindings.Add(EnhancedInputComponent->BindAction(RunKeyAction, ETriggerEvent::Triggered, this,
		                                                      &UFineMovementInputControl::OnRunKeyTriggered));
		ActionBindings.Add(EnhancedInputComponent->BindAction(RunKeyAction, ETriggerEvent::Completed, this,
		                                                      &UFineMovementInputControl::OnRunKeyReleased));
		ActionBindings.Add(EnhancedInputComponent->BindAction(RunKeyAction, ETriggerEvent::Canceled, this,
		                                                      &UFineMovementInputControl::OnRunKeyReleased));

		// Set up jump input events.
		ActionBindings.Add(EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this,
		                                                      &UFineMovementInputControl::OnInputStarted));
		ActionBindings.Add(EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this,
		                                                      &UFineMovementInputControl::OnJumpTriggered));
		ActionBindings.Add(EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this,
		                                                      &UFineMovementInputControl::OnJumpReleased));
		ActionBindings.Add(EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Canceled, this,
		                                                      &UFineMovementInputControl::OnJumpReleased));
	}
}

void UFineMovementInputControl::BindCharacterInputEvents()
{
	// Get ability system.
	UAbilitySystemComponent* AbilitySystem;
	if (!GetAbilitySystemComponent(AbilitySystem))
	{
		return;
	}
	// Bind to running tag change.
	AbilitySystem->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("Actor.State.Running")),
	                                        EGameplayTagEventType::NewOrRemoved).AddUObject(this,
		&UFineMovementInputControl::OnAbilitySystemTagChanged);
	// bind to jumping tag change.
	AbilitySystem->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("Actor.State.Jumping")),
	                                        EGameplayTagEventType::NewOrRemoved).AddUObject(this,
		&UFineMovementInputControl::OnAbilitySystemTagChanged);
}

void UFineMovementInputControl::UnbindCharacterInputEvents()
{
	// Get ability system.
	UAbilitySystemComponent* AbilitySystem;
	if (!GetAbilitySystemComponent(AbilitySystem))
	{
		return;
	}
	// Unbind to running tag change.
	AbilitySystem->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("Actor.State.Running")),
	                                        EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	// Unbind to jumping tag change.
	AbilitySystem->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("Actor.State.Jumping")),
	                                        EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
}

void UFineMovementInputControl::BeginPlay()
{
	Super::BeginPlay();
	// Create weak reference to this.
	const auto WeakThis = TWeakObjectPtr<UFineMovementInputControl>(this);
	// Set timer movement check
	auto& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(MovementTimerHandle,
	                      [=]()
	                      {
		                      if (WeakThis.IsValid())
		                      {
			                      const auto This = WeakThis.Get();
			                      // Get character from controlled pawn.
			                      const auto PlayerController = CastChecked<APlayerController>(
				                      This->GetOwner());
			                      const auto ControlledPawn = PlayerController->GetPawn();
			                      if (ControlledPawn)
			                      {
				                      const auto CharacterMovement = ControlledPawn->
					                      FindComponentByClass<UCharacterMovementComponent>();
				                      static const auto MovingTag = FGameplayTag::RequestGameplayTag(
					                      "Actor.State.Moving");
				                      UAbilitySystemComponent* AbilitySystem;
				                      if (!GetAbilitySystemComponent(AbilitySystem))
				                      {
					                      return;
				                      }
				                      // Check if character is moving.
				                      if (FMath::IsNearlyEqual(CharacterMovement->Velocity.Size(), 0.0f, 0.1f))
				                      {
					                      if (AbilitySystem->HasMatchingGameplayTag(MovingTag))
					                      {
						                      AbilitySystem->RemoveLooseGameplayTag(MovingTag);
						                      FP_VERBOSE("Removing moving tag.");
					                      }
				                      }
				                      else
				                      {
					                      if (!AbilitySystem->HasMatchingGameplayTag(MovingTag))
					                      {
						                      // Add Actor.State.Moving tag to ability system.
						                      AbilitySystem->AddLooseGameplayTag(MovingTag);
						                      FP_VERBOSE("Adding moving tag.");
					                      }
				                      }
			                      }
		                      }
	                      }, 0.1f, true);
}

void UFineMovementInputControl::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// remove movement timer
	GetWorld()->GetTimerManager().ClearTimer(MovementTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void UFineMovementInputControl::OnInputStarted()
{
	const auto PlayerController = CastChecked<APlayerController>(GetOwner());
	PlayerController->StopMovement();

	OnMovementStarted.Broadcast();
}

void UFineMovementInputControl::OnSetDestinationTriggered()
{
	if (!IsActive())
	{
		return;
	}
	const auto PlayerController = CastChecked<APlayerController>(GetOwner());

	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();

	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = PlayerController->GetHitResultUnderFinger(ETouchIndex::Touch1,
		                                                           ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		if (bIsTouch && TouchStart == FVector::ZeroVector)
		{
			TouchStart = Hit.Location;
		}
		CachedDestination = Hit.Location;
	}

	UpdateAddMovementInput();
}

void UFineMovementInputControl::OnSetDestinationReleased()
{
	if (!IsActive())
	{
		return;
	}
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		const auto PlayerController = CastChecked<APlayerController>(GetOwner());
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(PlayerController, CachedDestination);
		SpawnCursorEffect(CachedDestination);
	}

	FollowTime = 0.f;
	TouchStart = FVector::ZeroVector;
}

void UFineMovementInputControl::OnTouchTriggered()
{
	if (!IsActive())
	{
		return;
	}
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void UFineMovementInputControl::OnTouchReleased()
{
	if (!IsActive())
	{
		return;
	}
	bIsTouch = false;
	OnSetDestinationReleased();
}

void UFineMovementInputControl::OnWalkTriggered(const FInputActionInstance& InputActionInstance)
{
	if (!IsActive())
	{
		return;
	}
	// Get input value
	const FVector2D InputValue = InputActionInstance.GetValue().Get<FVector2D>();

	// Update cached destination by treating the input value as a direction from the current position.
	const auto PlayerController = CastChecked<APlayerController>(GetOwner());
	const FVector CurrentLocation = PlayerController->GetPawn()->GetActorLocation();
	const FVector2D InputDirection = InputValue.GetSafeNormal();
	const FVector InputDirection3D = FVector(InputDirection, 0.f);
	constexpr auto WalkInputMagnitude = 100.0f;
	const FVector NewDestination = CurrentLocation + InputDirection3D * WalkInputMagnitude;
	CachedDestination = NewDestination;

	UpdateAddMovementInput();
}

void UFineMovementInputControl::OnWalkReleased(const FInputActionInstance& InputActionInstance)
{
}

void UFineMovementInputControl::OnRunKeyTriggered()
{
	if (!IsActive())
	{
		return;
	}
	// clear run disable timer.
	GetWorld()->GetTimerManager().ClearTimer(RunDisableTimerHandle);

	if (!IsCharacterRunning())
	{
		UAbilitySystemComponent* AbilitySystemComponent;
		if (!GetAbilitySystemComponent(AbilitySystemComponent))
		{
			return;
		}
		AbilitySystemComponent->PressInputID(RunActionInputID);
	}
}

void UFineMovementInputControl::OnRunKeyReleased()
{
	if (!IsActive())
	{
		return;
	}
	UAbilitySystemComponent* AbilitySystemComponent;
	if (!GetAbilitySystemComponent(AbilitySystemComponent))
	{
		return;
	}
	AbilitySystemComponent->ReleaseInputID(RunActionInputID);
}

void UFineMovementInputControl::OnJumpTriggered()
{
	if (!IsActive())
	{
		return;
	}
	UAbilitySystemComponent* AbilitySystemComponent;
	if (!GetAbilitySystemComponent(AbilitySystemComponent))
	{
		return;
	}
	AbilitySystemComponent->PressInputID(JumpActionInputID);
	FP_LOG("Jump Triggered");
}

void UFineMovementInputControl::OnJumpReleased()
{
	if (!IsActive())
	{
		return;
	}
	UAbilitySystemComponent* AbilitySystemComponent;
	if (!GetAbilitySystemComponent(AbilitySystemComponent))
	{
		return;
	}
	AbilitySystemComponent->ReleaseInputID(JumpActionInputID);
	FP_LOG("Jump released.");
}

void UFineMovementInputControl::SpawnCursorEffect(const FVector& Location)
{
	if (!IsActive())
	{
		return;
	}
	if (IsValid(FXCursor))
	{
		const auto PlayerController = CastChecked<APlayerController>(GetOwner());
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(PlayerController, FXCursor, Location,
		                                               FRotator::ZeroRotator,
		                                               FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None,
		                                               true);
	}
	OnCursorEffectSpawned.Broadcast(Location);
}

void UFineMovementInputControl::OnAbilitySystemTagChanged(FGameplayTag Tag, int32 NewCount)
{
	const auto RunningTag = FGameplayTag::RequestGameplayTag(TEXT("Actor.State.Running"));
	const auto JumpingTag = FGameplayTag::RequestGameplayTag(TEXT("Actor.State.Jumping"));
	int32 TargetInputID = -1;
	if (Tag == RunningTag)
	{
		TargetInputID = RunActionInputID;
	}
	else if (Tag == JumpingTag)
	{
		TargetInputID = JumpActionInputID;
	}
	if (TargetInputID >= 0)
	{
		if (NewCount > 0)
		{
			// Get ability system.
			UAbilitySystemComponent* AbilitySystem;
			if (!GetAbilitySystemComponent(AbilitySystem))
			{
				return;
			}
			// Block run action input id.
			AbilitySystem->BlockAbilityByInputID(TargetInputID);
		}
		else
		{
			// Get ability system.
			UAbilitySystemComponent* AbilitySystem;
			if (!GetAbilitySystemComponent(AbilitySystem))
			{
				return;
			}
			// Unblock run action input id.
			AbilitySystem->UnBlockAbilityByInputID(TargetInputID);
		}
	}
}

bool UFineMovementInputControl::IsCharacterRunning()
{
	UAbilitySystemComponent* AbilitySystemComponent;
	if (!GetAbilitySystemComponent(AbilitySystemComponent))
	{
		return false;
	}
	const auto CharacterGameplay = GetCharacterGameplay();
	const auto TagRunning = FGameplayTag::RequestGameplayTag(CharacterGameplay->RunGameplayTagName);
	return AbilitySystemComponent->HasMatchingGameplayTag(TagRunning);
}

bool UFineMovementInputControl::IsCharacterJumping()
{
	UAbilitySystemComponent* AbilitySystemComponent;
	if (!GetAbilitySystemComponent(AbilitySystemComponent))
	{
		return false;
	}
	const auto CharacterGameplay = GetCharacterGameplay();
	const auto TagJumping = FGameplayTag::RequestGameplayTag(CharacterGameplay->JumpGameplayTagName);
	return AbilitySystemComponent->HasMatchingGameplayTag(TagJumping);
}

void UFineMovementInputControl::UpdateAddMovementInput()
{
	if (!IsActive())
	{
		return;
	}
	// Move towards mouse pointer or touch
	if (IsCharacterJumping())
	{
		return;
	}
	const auto PlayerController = CastChecked<APlayerController>(GetOwner());
	APawn* ControlledPawn = PlayerController->GetPawn();
	FVector Destination;
	if (bIsTouch)
	{
		Destination = CachedDestination - TouchStart + ControlledPawn->GetActorLocation();
	}
	else
	{
		Destination = CachedDestination;
	}
	if (ControlledPawn != nullptr)
	{
		// Don't add input if the character is jumping.
		// Get character gameplay component
		const auto CharacterGameplay = ControlledPawn->FindComponentByClass<UFineCharacterGameplay>();
		const FVector WorldDirection = Destination - ControlledPawn->GetActorLocation();
		if (WorldDirection.IsNearlyZero())
		{
			return;
		}
		constexpr float OffsetDistance = 5.f;
		const auto ForwardOffset = WorldDirection.GetSafeNormal() * OffsetDistance;
		const auto DistanceFromGround = CharacterGameplay->GetDistanceFromGroundStaticMesh(ForwardOffset);
		if (DistanceFromGround > OffsetDistance)
		{
			return;
		}
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}
