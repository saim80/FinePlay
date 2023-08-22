#include "Actor/FineMaterialParameterAnimation.h"

#include "FinePlayLog.h"
#include "Materials/MaterialInstanceConstant.h"

UFineMaterialParameterAnimation::UFineMaterialParameterAnimation(): Super()
{
}

void UFineMaterialParameterAnimation::ResetMaterialParameterValue()
{
	if (!MaterialInstanceDynamic.IsValid())
	{
		FP_WARNING("Material instance dynamic is not valid.");
		return;
	}
	StopAnimation();
	UpdateMaterialParameter();
}

void UFineMaterialParameterAnimation::PlayAnimation(float InStartValue, float InEndValue)
{
	if (!MaterialInstanceDynamic.IsValid())
	{
		FP_WARNING("Material instance dynamic is not valid.");
		return;
	}
	StartValue = InStartValue;
	EndValue = InEndValue;
	Elapsed = 0.f;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UFineMaterialParameterAnimation::UpdateMaterialParameter,
	                                       Interval, true);
	OnMaterialAnimationBegin.Broadcast(StartValue, EndValue);
}

void UFineMaterialParameterAnimation::StopAnimation()
{
	if (!MaterialInstanceDynamic.IsValid())
	{
		FP_WARNING("Material instance dynamic is not valid.");
		return;
	}
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

bool UFineMaterialParameterAnimation::IsAnimating() const
{
	// Check timer handle is valid.
	return TimerHandle.IsValid();
}

void UFineMaterialParameterAnimation::RunTestAnimation()
{
	PlayAnimation(0, 1);
}

void UFineMaterialParameterAnimation::BeginPlay()
{
	Super::BeginPlay();
	UpdateMeshComponent();
	ReplaceMaterialInstanceDynamic();
}

void UFineMaterialParameterAnimation::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RestoreMaterialInterface();
	MaterialInstanceDynamic = nullptr;
	MeshComponent = nullptr;
	Super::EndPlay(EndPlayReason);
}

void UFineMaterialParameterAnimation::ReplaceMaterialInstanceDynamic()
{
	// if material instance dynamic is not valid, create it.
	if (MaterialInstanceDynamic.IsValid())
	{
		return;
	}
	// Get material interface from mesh component.
	const auto MaterialInterface = GetTargetMaterialFromMeshComponent();
	// if material interface is not valid, return.
	if (!MaterialInterface)
	{
		return;
	}
	// cast material interface to material instance dynamic.
	MaterialInstanceDynamic = Cast<UMaterialInstanceDynamic>(MaterialInterface);
	// if material instance dynamic is not valid, create it.
	if (MaterialInstanceDynamic.IsValid())
	{
		return;
	}
	// cast material interface to material instance constant.
	const auto MaterialInstanceConstant = Cast<UMaterialInstanceConstant>(MaterialInterface);
	if (!IsValid(MaterialInstanceConstant))
	{
		FP_WARNING("Failed to cast material interface to material instance constant.");
		return;
	}
	OriginalMaterial = MaterialInstanceConstant;
	// create material instance dynamic from material instance constant.
	MaterialInstanceDynamic = MeshComponent->CreateDynamicMaterialInstance(
		GetMaterialSlotIndex(), MaterialInstanceConstant);
	check(MaterialInstanceDynamic.IsValid());
}

void UFineMaterialParameterAnimation::RestoreMaterialInterface() const
{
	if (!IsValid(OriginalMaterial))
	{
		FP_LOG("Original material is not valid. Nothing to restore");
		return;
	}
	MeshComponent->SetMaterial(GetMaterialSlotIndex(), OriginalMaterial);
}

void UFineMaterialParameterAnimation::UpdateMaterialParameter()
{
	if (!MaterialInstanceDynamic.IsValid())
	{
		return;
	}
	// if it's not animating, set the default value to the material parameter.
	if (!IsAnimating())
	{
		MaterialInstanceDynamic->SetScalarParameterValue(ParameterName, DefaultValue);
		return;
	}
	// Update elapsed time.
	Elapsed += Interval;
	// Get alpha value from elapsed time / duration
	const auto Alpha = FMath::Clamp<float>(Elapsed / Duration, 0.0f, 1.0f);
	if (FMath::IsNearlyEqual(Alpha, 1.f, UE_KINDA_SMALL_NUMBER))
	{
		// Animation completed. Stop animation.
		// Continue executing to set the final value.
		StopAnimation();
		OnMaterialAnimationEnd.Broadcast(StartValue, EndValue);
	}
	// Get parameter value from linear interpolation.
	const auto ParameterValue = FMath::Lerp<float>(StartValue, EndValue, Alpha);
	// Set parameter value to material instance dynamic.
	MaterialInstanceDynamic->SetScalarParameterValue(ParameterName, ParameterValue);
}

void UFineMaterialParameterAnimation::UpdateMeshComponent()
{
	// If target tag is given, use it to find mesh component from owner actor.
	if (TargetMeshTag != NAME_None)
	{
		const auto FoundComponents = GetOwner()->GetComponentsByTag(UMeshComponent::StaticClass(), TargetMeshTag);
		if (FoundComponents.Num() > 0)
		{
			MeshComponent = Cast<UMeshComponent>(FoundComponents[0]);
		}
	}
	else
	{
		// if no target tag is given, use the first mesh component from owner actor.
		MeshComponent = GetOwner()->FindComponentByClass<UMeshComponent>();
	}
}

int32 UFineMaterialParameterAnimation::GetMaterialSlotIndex() const
{
	int32 TargetIndex;
	// try to use material slot name to get material interface from mesh component.
	if (MaterialSlotName != NAME_None)
	{
		TargetIndex = MeshComponent->GetMaterialIndex(MaterialSlotName);
	}
	else if (MaterialSlotIndex >= 0)
	{
		TargetIndex = MaterialSlotIndex;
	}
	else
	{
		TargetIndex = 0;
	}
	return TargetIndex;
}

UMaterialInterface* UFineMaterialParameterAnimation::GetTargetMaterialFromMeshComponent() const
{
	return MeshComponent->GetMaterial(GetMaterialSlotIndex());
}
