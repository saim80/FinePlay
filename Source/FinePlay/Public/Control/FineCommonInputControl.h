#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FineCommonInputControl.generated.h"

class UAbilitySystemComponent;
class UFineCharacterGameplay;
class UInputAction;
struct FInputBindingHandle;
class UInputMappingContext;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FINEPLAY_API UFineCommonInputControl : public UActorComponent
{
	GENERATED_BODY()

public:
	UFineCommonInputControl();

	FORCEINLINE const int32& GetInteractActionInputID() const { return InteractActionInputID; }
	FORCEINLINE void SetInteractActionInputID(const int32& InInteractActionID)
	{
		InteractActionInputID = InInteractActionID;
	}

protected:
	virtual void SetupInputComponent();
	virtual void TearDownInputComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Activate(bool bReset) override;
	virtual void Deactivate() override;

	virtual void OnInputStarted();
	void OnInteractTriggered();
	void OnInteractReleased();

	UFineCharacterGameplay* GetCharacterGameplay() const;
	bool GetAbilitySystemComponent(UAbilitySystemComponent*& OutAbilitySystemComponent) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	TArray<FInputBindingHandle> ActionBindings;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	int32 InteractActionInputID;
};
