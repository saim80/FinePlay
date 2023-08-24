// (c) 2023 Pururum LLC. All rights reserved.


#include "Utilities/FinePaperColorAnimation.h"

#include "PaperFlipbookComponent.h"

UFinePaperColorAnimation::UFinePaperColorAnimation(): Super()
{
}

void UFinePaperColorAnimation::FadeIn()
{
	constexpr FLinearColor White = FLinearColor(1,1,1,1);
	constexpr FLinearColor Clear = FLinearColor(1, 1, 1, 0);
	PlayColorAnimation(Clear, White);
}

void UFinePaperColorAnimation::FadeOut()
{
	constexpr FLinearColor White = FLinearColor(1,1,1,1);
	constexpr FLinearColor Clear = FLinearColor(1, 1, 1, 0);
	PlayColorAnimation(White, Clear);
}

void UFinePaperColorAnimation::PlayColorAnimation(FLinearColor InStartColor, FLinearColor InEndColor)
{
	StartColor = InStartColor;
	EndColor = InEndColor;
	PlayAnimation();
}

void UFinePaperColorAnimation::BeginPlay()
{
	Super::BeginPlay();
	const auto Owner = GetOwner();
	if (Owner != nullptr)
	{
		FlipbookComponents.Empty();
		TArray<UPaperFlipbookComponent*> OutComponents;
		Owner->GetComponents(OutComponents);
		for (const auto Component : OutComponents)
		{
			FlipbookComponents.Add(Component);
		}
	}
}

void UFinePaperColorAnimation::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(AnimTimerHandle);
	FlipbookComponents.Empty();
	Super::EndPlay(EndPlayReason);
}

void UFinePaperColorAnimation::UpdateAnimation()
{
	Super::UpdateAnimation();
	const auto Alpha = Elapsed / Duration;
	const auto SourceColor = bReversed ? EndColor : StartColor;
	const auto TargetColor = bReversed ? StartColor : EndColor;
	const auto CurrentColor = FMath::Lerp(SourceColor, TargetColor, Alpha);
	for (const auto FlipbookComponent : FlipbookComponents)
	{
		if (FlipbookComponent.IsValid())
		{
			FlipbookComponent->SetSpriteColor(CurrentColor);
		}
	}
}
