#pragma once
#include "CoreMinimal.h"
#include "Utilities/FineAnimationComponent.h"
#include "FineMaterialParameterAnimation.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaterialAnimationBegin, float, StartValue, float, EndValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaterialAnimationEnd, float, StartValue, float, EndValue);

UCLASS(Blueprintable, BlueprintType, ClassGroup = (FinePlay), meta = (BlueprintSpawnableComponent))
class FINEPLAY_API UFineMaterialParameterAnimation : public UFineAnimationComponent
{
	GENERATED_BODY()

public:
	UFineMaterialParameterAnimation();

	UFUNCTION(BlueprintCallable)
	void ResetMaterialParameterValue();

	UFUNCTION(BlueprintCallable)
	void PlayMaterialAnimation(float InStartValue, float InEndValue);

#if WITH_EDITOR
	UFUNCTION(BlueprintCallable, CallInEditor)
	void RunTestAnimation();
#endif

	UPROPERTY(BlueprintAssignable)
	FOnMaterialAnimationBegin OnMaterialAnimationBegin;
	UPROPERTY(BlueprintAssignable)
	FOnMaterialAnimationEnd OnMaterialAnimationEnd;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void UpdateAnimation() override;

private:
	void ReplaceMaterialInstanceDynamic();
	void RestoreMaterialInterface() const;
	void UpdateMaterialParameter();
	void UpdateMeshComponent();
	int32 GetMaterialSlotIndex() const;
	UMaterialInterface* GetTargetMaterialFromMeshComponent() const;

	float StartValue = 0.f;
	float EndValue = 1.f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineMaterialParameterAnimation",
		meta = (AllowPrivateAccess = true))
	float DefaultValue = 0.f;

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
