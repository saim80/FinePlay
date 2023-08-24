// (c) 2023 Pururum LLC. All rights reserved.


#include "Utilities/FineTransformAnimation.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetNodeHelperLibrary.h"


// Sets default values for this component's properties
UFineTransformAnimation::UFineTransformAnimation(): Super()
{
}

void UFineTransformAnimation::PlayShrinkToCenterAnimation()
{
	auto TargetTransform = OriginalTransform;
	TargetTransform.SetScale3D(FVector(0, 0, 0));
	PlayTransformAnimation(OriginalTransform, TargetTransform);
}

void UFineTransformAnimation::PlayExpandFromCenterAnimation()
{
	auto SourceTransform = OriginalTransform;
	SourceTransform.SetScale3D(FVector(0, 0, 0));
	PlayTransformAnimation(SourceTransform, OriginalTransform);
}

void UFineTransformAnimation::PlayTransformAnimation(FTransform InStartTransform, FTransform InEndTransform)
{
	StartTransform = InStartTransform;
	EndTransform = InEndTransform;
	PlayAnimation();
}

void UFineTransformAnimation::BeginPlay()
{
	Super::BeginPlay();
	const auto Owner = GetOwner();
	if (Owner != nullptr)
	{
		const auto Found = Owner->GetComponentsByTag(USceneComponent::StaticClass(), TargetComponentTag);
		if (!Found.IsEmpty())
		{
			TargetComponent = Cast<USceneComponent>(Found[0]);
			if (bRelative)
			{
				OriginalTransform = TargetComponent->GetRelativeTransform();
			}
			else
			{
				OriginalTransform = TargetComponent->GetComponentTransform();
			}
		}
	}
}

void UFineTransformAnimation::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	TargetComponent = nullptr;
	Super::EndPlay(EndPlayReason);
}

void UFineTransformAnimation::UpdateAnimation()
{
	Super::UpdateAnimation();
	const auto Alpha = Elapsed / Duration;
	const auto SourceTransform = bReversed ? EndTransform : StartTransform;
	const auto TargetTransform = bReversed ? StartTransform : EndTransform;
	FTransform NewTransform;
	FRotator SourceRotator;
	SourceRotator.Pitch = SourceTransform.Rotator().Pitch;
	SourceRotator.Yaw = SourceTransform.Rotator().Yaw;
	SourceRotator.Roll = SourceTransform.Rotator().Roll;
	FRotator TargetRotator;
	TargetRotator.Pitch = TargetTransform.Rotator().Pitch;
	TargetRotator.Yaw = TargetTransform.Rotator().Yaw;
	TargetRotator.Roll = TargetTransform.Rotator().Roll;
	NewTransform.SetLocation(FMath::Lerp(SourceTransform.GetLocation(), TargetTransform.GetLocation(), Alpha));
	NewTransform.SetRotation(
		UKismetMathLibrary::RLerp(SourceRotator, TargetRotator, Alpha, true).Quaternion());
	NewTransform.SetScale3D(FMath::Lerp(SourceTransform.GetScale3D(), TargetTransform.GetScale3D(), Alpha));
	if (TargetComponent.IsValid())
	{
		if (bRelative)
		{
			TargetComponent->SetRelativeTransform(NewTransform);
		}
		else
		{
			TargetComponent->SetWorldTransform(NewTransform);
		}
	}
}
