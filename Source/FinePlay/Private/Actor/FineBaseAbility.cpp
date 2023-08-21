// (c) 2023 Pururum LLC. All rights reserved.


#include "Actor/FineBaseAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "FinePlayLog.h"


bool UFineBaseAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayTagContainer* SourceTags,
                                          const FGameplayTagContainer* TargetTags,
                                          FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	static const auto AliveTag = FGameplayTag::RequestGameplayTag(FName("Actor.State.Alive"));
	const auto AbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ActorInfo->AvatarActor.Get());
	const auto CanActivate = AbilitySystem->HasMatchingGameplayTag(AliveTag);
	FP_LOG("CanActivate %s ? %s", *GetClass()->GetDisplayNameText().ToString(),
	       CanActivate ? TEXT("true") : TEXT("false"));
	return CanActivate;
}

void UFineBaseAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		return;
	}
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	const auto EffectClasses = GetEffectClasses();
	if (EffectClasses.IsEmpty())
	{
		FP_ERROR("EffectClass is null.");
		return;
	}
	for (const auto& EffectClass : EffectClasses)
	{
		if (!EffectClass)
		{
			FP_ERROR("EffectClass is null.");
			return;
		}

		const auto NewSpec = MakeOutgoingGameplayEffectSpec(EffectClass, GetAbilityLevel(Handle, ActorInfo));
		EffectHandles.Add(ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo,
		                                               MakeOutgoingGameplayEffectSpec(
			                                               EffectClass, GetAbilityLevel(Handle, ActorInfo))));
		FP_LOG("Apply %s effect to owner", *EffectClass->GetDisplayNameText().ToString());
	}
}

void UFineBaseAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

void UFineBaseAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UFineBaseAbility::CancelAbility, Handle,
		                                                      ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	for (const auto EffectHandle : EffectHandles)
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(EffectHandle);
	}

	FP_LOG("%s effect removed.", *GetClass()->GetDisplayNameText().ToString());
}

void UFineBaseAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                  const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo,
                                  bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	for (const auto EffectHandle : EffectHandles)
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(EffectHandle);
	}
}

TArray<TSubclassOf<UGameplayEffect>> UFineBaseAbility::GetEffectClasses()
{
	return {};
}
