﻿// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FinePlayerController.generated.h"

class UFineMovementInputControl;
/**
 * Provides a base class for player controllers.
 */
UCLASS()
class FINEPLAY_API AFinePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AFinePlayerController();
	
protected:
	virtual void BeginPlay() override;
	
};