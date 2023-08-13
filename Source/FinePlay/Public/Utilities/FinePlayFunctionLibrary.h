// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FinePlayFunctionLibrary.generated.h"

class UFineSceneLoop;
/**
 * A set of utility functions for fine play module.
 */
UCLASS()
class FINEPLAY_API UFinePlayFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FinePlay")
	static UFineSceneLoop* GetSceneLoop(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FinePlay")
	static bool IsStreamingCompleted(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "FinePlay")
	static bool IsStreamingNeeded(const UObject* WorldContextObject);
};
