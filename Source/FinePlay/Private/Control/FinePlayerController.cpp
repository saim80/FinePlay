// (c) 2023 Pururum LLC. All rights reserved.


#include "Control/FinePlayerController.h"
#include "GameFramework/Pawn.h"
#include "Control/FineMovementInputControl.h"

AFinePlayerController::AFinePlayerController(): Super()
{
	SetShowMouseCursor(true);
	DefaultMouseCursor = EMouseCursor::Default;
}

void AFinePlayerController::BeginPlay()
{
	Super::BeginPlay();

	auto InputMode = FInputModeGameAndUI();
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}
