// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FineAnimationComponent.h"
#include "Components/ActorComponent.h"
#include "FineTransformAnimation.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FINEPLAY_API UFineTransformAnimation : public UFineAnimationComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFineTransformAnimation();

	UFUNCTION(BlueprintCallable)
	void PlayShrinkToCenterAnimation();
	UFUNCTION(BlueprintCallable)
	void PlayExpandFromCenterAnimation();

	UFUNCTION(BlueprintCallable)
	void PlayTransformAnimation(FTransform InStartTransform, FTransform InEndTransform);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void UpdateAnimation() override;

private:
	UPROPERTY(BlueprintReadOnly, Category = "FinePlay", meta = (AllowPrivateAccess = "true"))
	FTransform StartTransform;
	UPROPERTY(BlueprintReadOnly, Category = "FinePlay", meta = (AllowPrivateAccess = "true"))
	FTransform EndTransform;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FinePlay", meta = (AllowPrivateAccess = "true"))
	bool bRelative;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FinePlay", meta = (AllowPrivateAccess = "true"))
	FName TargetComponentTag;

	TWeakObjectPtr<USceneComponent> TargetComponent;

	FTransform OriginalTransform;
};
