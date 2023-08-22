#pragma once
#include "CoreMinimal.h"
#include "FineMaterialParameterAnimation.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaterialAnimationBegin, float, StartValue, float, EndValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaterialAnimationEnd, float, StartValue, float, EndValue);

UCLASS(Blueprintable, BlueprintType, ClassGroup = (FinePlay), meta = (BlueprintSpawnableComponent))
class FINEPLAY_API UFineMaterialParameterAnimation : public UActorComponent
{
	GENERATED_BODY()

public:
	UFineMaterialParameterAnimation();

	UFUNCTION(BlueprintCallable)
	void ResetMaterialParameterValue();

	UFUNCTION(BlueprintCallable)
	void PlayAnimation(float InStartValue, float InEndValue);

	UFUNCTION(BlueprintCallable)
	void StopAnimation();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAnimating() const;

#if WITH_EDITOR
	UFUNCTION(BlueprintCallable, CallInEditor)
	void RunTestAnimation();
#endif

	const float& GetDuration() const { return Duration; }

	UPROPERTY(BlueprintAssignable)
	FOnMaterialAnimationBegin OnMaterialAnimationBegin;
	UPROPERTY(BlueprintAssignable)
	FOnMaterialAnimationEnd OnMaterialAnimationEnd;
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void ReplaceMaterialInstanceDynamic();
	void RestoreMaterialInterface() const;
	void UpdateMaterialParameter();
	void UpdateMeshComponent();
	int32 GetMaterialSlotIndex() const;
	UMaterialInterface* GetTargetMaterialFromMeshComponent() const;

	float Elapsed;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineMaterialParameterAnimation",
		meta = (AllowPrivateAccess = true))
	float Duration = 0.5f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineMaterialParameterAnimation",
		meta = (AllowPrivateAccess = true))
	float Interval = 1.f / 30.f;
	float StartValue = 0.f;
	float EndValue = 1.f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineMaterialParameterAnimation",
		meta = (AllowPrivateAccess = true))
	float DefaultValue = 0.f;

	FTimerHandle TimerHandle;

	UPROPERTY(Transient, meta = (AllowPrivateAccess = true))
	TWeakObjectPtr<UMaterialInstanceDynamic> MaterialInstanceDynamic;
	UPROPERTY(Transient, meta = (AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> OriginalMaterial;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineMaterialParameterAnimation",
		meta = (AllowPrivateAccess = true))
	FName ParameterName = "Opacity";

	UPROPERTY(Transient, meta = (AllowPrivateAccess = true))
	TWeakObjectPtr<UMeshComponent> MeshComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineMaterialParameterAnimation",
		meta = (AllowPrivateAccess = true))
	FName TargetMeshTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineMaterialParameterAnimation",
		meta = (AllowPrivateAccess = true))
	FName MaterialSlotName = NAME_None;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineMaterialParameterAnimation",
		meta = (AllowPrivateAccess = true))
	int32 MaterialSlotIndex = -1;
};
