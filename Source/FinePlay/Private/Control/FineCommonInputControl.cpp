#include "Control/FineCommonInputControl.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FinePlayLog.h"
#include "Actor/FineCharacterGameplay.h"


UFineCommonInputControl::UFineCommonInputControl(): Super()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFineCommonInputControl::SetupInputComponent()
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
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		ActionBindings.Add(EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this,
			&UFineCommonInputControl::OnInputStarted));
		ActionBindings.Add(EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, 
			&UFineCommonInputControl::OnInteractTriggered));
		ActionBindings.Add(EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, 
			&UFineCommonInputControl::OnInteractReleased));
		ActionBindings.Add(EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Canceled, this, 
			&UFineCommonInputControl::OnInteractReleased));
	}
}

void UFineCommonInputControl::TearDownInputComponent()
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
		ActionBindings.Empty();
	}
}

void UFineCommonInputControl::BeginPlay()
{
	Super::BeginPlay();
}

void UFineCommonInputControl::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UFineCommonInputControl::Activate(bool bReset)
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

void UFineCommonInputControl::Deactivate()
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

void UFineCommonInputControl::OnInputStarted()
{
	FP_LOG("Common input started");
}

void UFineCommonInputControl::OnInteractTriggered()
{
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;
	if (GetAbilitySystemComponent(AbilitySystemComponent))
	{
		AbilitySystemComponent->PressInputID(InteractActionInputID);
		FP_LOG("Interaction triggered");
	}
}

void UFineCommonInputControl::OnInteractReleased()
{
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;
	if (GetAbilitySystemComponent(AbilitySystemComponent))
	{
		AbilitySystemComponent->ReleaseInputID(InteractActionInputID);
		FP_LOG("Interaction released");
	}
}

UFineCharacterGameplay* UFineCommonInputControl::GetCharacterGameplay() const
{
	const auto PlayerController = CastChecked<APlayerController>(GetOwner());
	const auto Pawn = PlayerController->GetPawn();
	if (!IsValid(Pawn))
	{
		return nullptr;
	}
	return Pawn->FindComponentByClass<UFineCharacterGameplay>();
}

bool UFineCommonInputControl::GetAbilitySystemComponent(
	UAbilitySystemComponent*& OutAbilitySystemComponent) const
{
	if (const UFineCharacterGameplay* CharacterGameplay = GetCharacterGameplay())
	{
		const auto Pawn = CharacterGameplay->GetOwner();
		OutAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn);
		return true;
	}
	OutAbilitySystemComponent = nullptr;
	return false;
}
