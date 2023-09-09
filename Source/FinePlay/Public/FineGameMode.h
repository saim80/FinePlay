// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FineGameMode.generated.h"

/**
 * Game mode that takes scene actor into account.
 */
UCLASS()
class FINEPLAY_API AFineGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual bool PlayerCanRestart_Implementation(APlayerController* Player) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
};
