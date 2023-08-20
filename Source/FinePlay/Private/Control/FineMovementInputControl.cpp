// (c) 2023 Pururum LLC. All rights reserved.


#include "Control/FineMovementInputControl.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraFunctionLibrary.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"


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
	// Get owner as player controller
	const auto PlayerController = CastChecked<APlayerController>(GetOwner());

	// Get input component from player controller
	const auto InputComponent = PlayerController->InputComponent;
	if (!IsValid(InputComponent))
	{
		return;
	}
	if (!ActionBindings.IsEmpty())
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

		// Setup run touch input events.
		ActionBindings.Add(EnhancedInputComponent->BindAction(RunTouchAction, ETriggerEvent::Started, this,
		                                                      &UFineMovementInputControl::OnInputStarted));
		ActionBindings.Add(EnhancedInputComponent->BindAction(RunTouchAction, ETriggerEvent::Triggered, this,
		                                                      &UFineMovementInputControl::OnRunTouchTriggered));
		ActionBindings.Add(EnhancedInputComponent->BindAction(RunTouchAction, ETriggerEvent::Completed, this,
		                                                      &UFineMovementInputControl::OnRunTouchReleased));
		ActionBindings.Add(EnhancedInputComponent->BindAction(RunTouchAction, ETriggerEvent::Canceled, this,
		                                                      &UFineMovementInputControl::OnRunTouchReleased));
	}
}

void UFineMovementInputControl::TearDownInputComponent()
{
	const auto PlayerController = CastChecked<APlayerController>(GetOwner());
	const auto InputComponent = PlayerController->InputComponent;
	if (ActionBindings.IsEmpty())
	{
		return;
	}
	if (!IsValid(InputComponent))
	{
		return;
	}
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// iterate ActionBindings and remove them from the input component
		for (const auto& ActionBinding : ActionBindings)
		{
			EnhancedInputComponent->RemoveBinding(ActionBinding);
		}
	}
}

void UFineMovementInputControl::OnInputStarted()
{
	const auto PlayerController = CastChecked<APlayerController>(GetOwner());
	PlayerController->StopMovement();

	OnMovementStarted.Broadcast();
}

void UFineMovementInputControl::OnSetDestinationTriggered()
{
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
		CachedDestination = Hit.Location;
	}

	// Move towards mouse pointer or touch
	APawn* ControlledPawn = PlayerController->GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void UFineMovementInputControl::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		const auto PlayerController = CastChecked<APlayerController>(GetOwner());
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(PlayerController, CachedDestination);
		SpawnCursorEffect(CachedDestination);
	}

	FollowTime = 0.f;
}

void UFineMovementInputControl::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void UFineMovementInputControl::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void UFineMovementInputControl::OnWalkTriggered(const FInputActionInstance& InputActionInstance)
{
	// Get input value
	const FVector2D InputValue = InputActionInstance.GetValue().Get<FVector2D>();

	CachedDestination.X = InputValue.X;
	CachedDestination.Y = InputValue.Y;

	// Add movement input to the controller.
	const auto PlayerController = CastChecked<APlayerController>(GetOwner());
	APawn* ControlledPawn = PlayerController->GetPawn();
	if (ControlledPawn != nullptr)
	{
		ControlledPawn->AddMovementInput(CachedDestination, 1.0, false);
	}
}

void UFineMovementInputControl::OnWalkReleased(const FInputActionInstance& InputActionInstance)
{
	CachedDestination = FVector::ZeroVector;
}

void UFineMovementInputControl::OnRunKeyTriggered()
{
	// clear run disable timer.
	GetWorld()->GetTimerManager().ClearTimer(RunDisableTimerHandle);
	
	UAbilitySystemComponent* AbilitySystemComponent;
	if (!GetAbilitySystemComponent(AbilitySystemComponent))
	{
		return;
	}
	if (!IsCharacterRunning(AbilitySystemComponent))
	{
		AbilitySystemComponent->PressInputID(RunActionInputID);
	}
}

void UFineMovementInputControl::OnRunKeyReleased()
{
	UAbilitySystemComponent* AbilitySystemComponent;
	if (!GetAbilitySystemComponent(AbilitySystemComponent))
	{
		return;
	}
	if (IsCharacterRunning(AbilitySystemComponent))
	{
		AbilitySystemComponent->ReleaseInputID(RunActionInputID);
	}
}

void UFineMovementInputControl::OnRunTouchTriggered()
{
	OnRunKeyTriggered();
}

void UFineMovementInputControl::OnRunTouchReleased()
{
	// if it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// create weak this
		TWeakObjectPtr<UFineMovementInputControl> WeakThis(this);
		// Start run disable timer.
		GetWorld()->GetTimerManager().SetTimer(RunDisableTimerHandle, [=]()
		{
			// if weak this is still valid
			if (!WeakThis.IsValid())
			{
				return;
			}
			const auto This = WeakThis.Get();
			// Check if player is still running
			UAbilitySystemComponent* AbilitySystemComponent;
			if (!This->GetAbilitySystemComponent(AbilitySystemComponent))
			{
				return;
			}
			if (!This->IsCharacterRunning(AbilitySystemComponent))
			{
				// clear timer
				This->GetWorld()->GetTimerManager().ClearTimer(This->RunDisableTimerHandle);
				return;
			}
			// track the destination distance periodically and quit running if it's too close
			const auto PlayerController = CastChecked<APlayerController>(This->GetOwner());
			const APawn* ControlledPawn = PlayerController->GetPawn();
			if (ControlledPawn != nullptr)
			{
				const float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), This->CachedDestination);
				if (Distance <= 5.0f)
				{
					This->OnRunKeyReleased();
					// Clear timer
					This->GetWorld()->GetTimerManager().ClearTimer(This->RunDisableTimerHandle);
				}
			}
		}, 0.1f, true);
	}
	else
	{
		OnRunKeyReleased();
	}
}

void UFineMovementInputControl::Activate(bool bReset)
{
	Super::Activate(bReset);

	// Get owner as player controller
	const auto PlayerController = CastChecked<APlayerController>(GetOwner());
	const auto LocalPlayer = PlayerController->GetLocalPlayer();
	if (IsValid(LocalPlayer))
	{
		const auto LocalInputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		// Add input mapping context
		if (LocalInputSubsystem)
		{
			LocalInputSubsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	SetupInputComponent();
}

void UFineMovementInputControl::Deactivate()
{
	const auto PlayerController = CastChecked<APlayerController>(GetOwner());
	TearDownInputComponent();

	const auto LocalPlayer = PlayerController->GetLocalPlayer();
	if (IsValid(LocalPlayer))
	{
		const auto LocalInputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		// Remove input mapping context
		if (LocalInputSubsystem)
		{
			LocalInputSubsystem->RemoveMappingContext(DefaultMappingContext);
		}
	}

	Super::Deactivate();
}

void UFineMovementInputControl::SpawnCursorEffect(const FVector& Location)
{
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

bool UFineMovementInputControl::GetAbilitySystemComponent(OUT UAbilitySystemComponent*& OutComponent)
{
	// Get player controller. Assume the owner is the one.
	const auto PlayerController = CastChecked<APlayerController>(GetOwner());
	// Get pawn
	APawn* ControlledPawn = PlayerController->GetPawn();
	// Get ability system component from the pawn
	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		ControlledPawn);
	// If ability system component is invalid, return
	if (!IsValid(AbilitySystemComponent))
	{
		OutComponent = nullptr;
		return false;
	}
	OutComponent = AbilitySystemComponent;
	return true;
}

bool UFineMovementInputControl::IsCharacterRunning(const UAbilitySystemComponent* AbilitySystemComponent)
{
	// Check if the ability system already has Actor.State.Running tag.
	const auto TagRunning = FGameplayTag::RequestGameplayTag("Actor.State.Running");
	return AbilitySystemComponent->HasMatchingGameplayTag(TagRunning);
}
