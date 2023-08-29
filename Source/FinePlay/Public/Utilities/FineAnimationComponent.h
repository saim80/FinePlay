// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FineAnimationComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnimationStateUpdated);

/**
 * 
 */
UCLASS(Category = "FinePlay", meta = (BlueprintSpawnableComponent))
class FINEPLAY_API UFineAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFineAnimationComponent();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PlayAnimation();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StopAnimation();

	UPROPERTY(BlueprintAssignable)
	FOnAnimationStateUpdated OnAnimationStarted;
	UPROPERTY(BlueprintAssignable)
	FOnAnimationStateUpdated OnAnimationFinished;
	UPROPERTY(BlueprintAssignable)
	FOnAnimationStateUpdated OnAnimationLoopStarted; // one strip of anim, a part of bigger animation.
	UPROPERTY(BlueprintAssignable)
	FOnAnimationStateUpdated OnAnimationLoopEnded;

	FORCEINLINE const float& GetDuration() const { return Duration; }
	FORCEINLINE void SetDuration(const float& NewDuration) { Duration = NewDuration; }
	FORCEINLINE bool GetAutoReverseOnLoop() const { return bAutoReverseOnLoop; }
	FORCEINLINE void SetAutoReverseOnLoop(const bool& bNewAutoReverseOnLoop)
	{
		bAutoReverseOnLoop = bNewAutoReverseOnLoop;
	}
	FORCEINLINE const int32& GetLoopCount() const { return LoopCount; }
	FORCEINLINE void SetLoopCount(const int32& NewLoopCount) { LoopCount = NewLoopCount; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAnimating();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void UpdateAnimation();

	bool bReversed = false;

	float Elapsed = 0;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FinePlay", meta = (AllowPrivateAccess = "true"))
	float Duration = 0.5;

	FTimerHandle AnimTimerHandle;

private:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FinePlay", meta = (AllowPrivateAccess = "true"))
	float Interval = 1.f / 30.f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FinePlay", meta = (AllowPrivateAccess = "true"))
	int32 LoopCount = 0; // infinite
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FinePlay", meta = (AllowPrivateAccess = "true"))
	bool bAutoReverseOnLoop = false;

	int32 CurrentLoopCount = 0;
};
