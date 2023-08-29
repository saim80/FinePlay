// (c) 2023 Pururum LLC. All rights reserved.


#include "Utilities/FineAnimationComponent.h"

UFineAnimationComponent::UFineAnimationComponent(): Super()
{
}

void UFineAnimationComponent::PlayAnimation_Implementation()
{
	Elapsed = 0;
	bReversed = false;
	CurrentLoopCount = 0;
	OnAnimationStarted.Broadcast();
	OnAnimationLoopStarted.Broadcast();
	GetWorld()->GetTimerManager().SetTimer(AnimTimerHandle, this, &UFineAnimationComponent::UpdateAnimation, Interval,
	                                       true);
}

void UFineAnimationComponent::StopAnimation_Implementation()
{
	if (!IsAnimating())
	{
		return;
	}
	OnAnimationFinished.Broadcast();
	GetWorld()->GetTimerManager().ClearTimer(AnimTimerHandle);
}

bool UFineAnimationComponent::IsAnimating()
{
	return AnimTimerHandle.IsValid();
}

void UFineAnimationComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UFineAnimationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UFineAnimationComponent::UpdateAnimation()
{
	Elapsed = FMath::Clamp(Elapsed + Interval, 0, Duration);
	if (FMath::IsNearlyEqual(Elapsed, Duration))
	{
		OnAnimationLoopEnded.Broadcast();
		if (LoopCount > 0)
		{
			CurrentLoopCount++;
			if (CurrentLoopCount == LoopCount)
			{
				StopAnimation();
				return;
			}
		}
		Elapsed = 0;
		if (bAutoReverseOnLoop)
		{
			bReversed = !bReversed;
		}
	}
}
