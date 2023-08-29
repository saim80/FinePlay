#include "Utilities/FineMaterialParameterAnimation.h"

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
	// for each default scalar value, set it to material instance dynamic.
	for (const auto& Pair : DefaultScalarValues)
	{
		const auto _ParameterName = Pair.Key;
		const auto DefaultValue = Pair.Value;
		MaterialInstanceDynamic->SetScalarParameterValue(_ParameterName, DefaultValue);
	}
	// for each default vector value, set it to material instance dynamic.
	for (const auto& Pair : DefaultVectorValues)
	{
		const auto _ParameterName = Pair.Key;
		const auto DefaultValue = Pair.Value;
		MaterialInstanceDynamic->SetVectorParameterValue(_ParameterName, DefaultValue);
	}
}

void UFineMaterialParameterAnimation::PlayMaterialAnimation(float InStartValue, float InEndValue)
{
	if (!MaterialInstanceDynamic.IsValid())
	{
		FP_WARNING("Material instance dynamic is not valid.");
		return;
	}
	StartValue = InStartValue;
	EndValue = InEndValue;
	OnMaterialAnimationBegin.Broadcast(StartValue, EndValue);
	UpdateMaterialParameter();
	PlayAnimation();
}

void UFineMaterialParameterAnimation::PlayMaterialMultiParameterAnimation(const TMap<FName, float>& SourceScalars,
                                                                          const TMap<FName, FLinearColor>&
                                                                          SourceVectors,
                                                                          const TMap<FName, float>& TargetScalars,
                                                                          const TMap<FName, FLinearColor>&
                                                                          TargetVectors)
{
	if (!MaterialInstanceDynamic.IsValid())
	{
		FP_WARNING("Material instance dynamic is not valid.");
		return;
	}
	SourceScalarValues = SourceScalars;
	SourceVectorValues = SourceVectors;
	TargetScalarValues = TargetScalars;
	TargetVectorValues = TargetVectors;
	StartValue = 0;
	EndValue = 1;
	OnMaterialAnimationBegin.Broadcast(StartValue, EndValue);
	UpdateMaterialParameter();
	PlayAnimation();
}

void UFineMaterialParameterAnimation::RunTestAnimation()
{
	PlayMaterialAnimation(0, 1);
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

void UFineMaterialParameterAnimation::UpdateAnimation()
{
	Super::UpdateAnimation();
	UpdateMaterialParameter();
}

void UFineMaterialParameterAnimation::StopAnimation_Implementation()
{
	Super::StopAnimation_Implementation();
	SourceScalarValues.Empty();
	SourceVectorValues.Empty();
	TargetScalarValues.Empty();
	TargetVectorValues.Empty();
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
	ResetMaterialParameterValue();
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
	// Get alpha value from elapsed time / duration
	const auto Alpha = FMath::Clamp<float>(Elapsed / Duration, 0.0f, 1.0f);
	if (FMath::IsNearlyEqual(Alpha, 1.f, UE_KINDA_SMALL_NUMBER))
	{
		// Animation completed. Stop animation.
		// Continue executing to set the final value.
		OnMaterialAnimationEnd.Broadcast(StartValue, EndValue);
	}
	if (SourceScalarValues.IsEmpty() && SourceVectorValues.IsEmpty())
	{
		// Get parameter value from linear interpolation.
		const auto ParameterValue = FMath::Lerp<float>(StartValue, EndValue, Alpha);
		// Set parameter value to material instance dynamic.
		MaterialInstanceDynamic->SetScalarParameterValue(ParameterName, ParameterValue);
	}
	else
	{
		check(SourceScalarValues.Num() == TargetScalarValues.Num());
		check(SourceVectorValues.Num() == TargetVectorValues.Num());

		// for each scalar value, get source and target value from map.
		// lerp to get the current value and set it to material instance dynamic.
		for (const auto& Pair : SourceScalarValues)
		{
			const auto _ParameterName = Pair.Key;
			const auto SourceValue = Pair.Value;
			const auto TargetValue = TargetScalarValues[_ParameterName];
			const auto ParameterValue = FMath::Lerp<float>(SourceValue, TargetValue, Alpha);
			MaterialInstanceDynamic->SetScalarParameterValue(_ParameterName, ParameterValue);
		}
		// for each linear color value, get source and target value from map.
		// lerp to get the current value and set it to material instance dynamic.
		for (const auto& Pair : SourceVectorValues)
		{
			const auto _ParameterName = Pair.Key;
			const auto SourceValue = Pair.Value;
			const auto TargetValue = TargetVectorValues[_ParameterName];
			const auto ParameterValue = FMath::Lerp<FLinearColor>(SourceValue, TargetValue, Alpha);
			MaterialInstanceDynamic->SetVectorParameterValue(_ParameterName, ParameterValue);
		}
	}
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
