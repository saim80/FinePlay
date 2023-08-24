// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FineAnimationComponent.h"
#include "UObject/Object.h"
#include "FinePaperColorAnimation.generated.h"

class UPaperFlipbookComponent;
/**
 * 
 */
UCLASS(Category= "FinePlay", meta = (BlueprintSpawnableComponent))
class FINEPLAY_API UFinePaperColorAnimation : public UFineAnimationComponent
{
	GENERATED_BODY()

public:
	UFinePaperColorAnimation();

	UFUNCTION(BlueprintCallable)
	void FadeIn();
	UFUNCTION(BlueprintCallable)
	void FadeOut();

	UFUNCTION(BlueprintCallable)
	void PlayColorAnimation(FLinearColor InStartColor, FLinearColor InEndColor);
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void UpdateAnimation() override;

private:
	UPROPERTY(BlueprintReadOnly, Category = "FinePlay", meta = (AllowPrivateAccess = "true"))
	FLinearColor EndColor;
	UPROPERTY(BlueprintReadOnly, Category = "FinePlay", meta = (AllowPrivateAccess = "true"))
	FLinearColor StartColor;

	TArray<TWeakObjectPtr<UPaperFlipbookComponent>> FlipbookComponents;
};
