// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FineMaterialParameterCollectionUpdater.generated.h"

struct FStreamableHandle;
class UMaterialParameterCollection;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FINEPLAY_API UFineMaterialParameterCollectionUpdater : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFineMaterialParameterCollectionUpdater();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UpdateMaterialParameterCollection();

	UFUNCTION(BlueprintCallable)
	void BeginUpdating();
	
	UFUNCTION(BlueprintCallable)
	void EndUpdating();
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsUpdating() const { return UpdateTimerHandle.IsValid(); }
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(BlueprintReadWrite, Category = "FinePlay", meta = (AllowPrivateAccess = "true"))
	TMap<FString, float> FloatParameters;
	UPROPERTY(BlueprintReadWrite, Category = "FinePlay", meta = (AllowPrivateAccess = "true"))
	TMap<FString, FLinearColor> VectorParameters;

private:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FinePlay", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UMaterialParameterCollection> MaterialParameterCollectionAsset;
	UPROPERTY(BlueprintReadOnly, Category = "FinePlay", meta = (AllowPrivateAccess = "true"))
	UMaterialParameterCollection* MaterialParameterCollection;

	TSharedPtr<FStreamableHandle> StreamableHandle;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FinePlay", meta = (AllowPrivateAccess = "true"))
	float UpdateInterval = 0.1f;
	FTimerHandle UpdateTimerHandle;
};
