// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Abilities/GA_Dash.h"
#include "Abilities/System/GameGameplayTags.h"
#include "GameFramework/Character.h"

UGA_Dash::UGA_Dash()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	AbilityTags.AddTag(GameGameplayTags::AbilityTag_Movement_Dash);
	ActivationOwnedTags.AddTag(GameGameplayTags::AbilityTag_Movement_Dash);
}

void UGA_Dash::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    ACharacter* Char = Cast<ACharacter>(ActorInfo ? ActorInfo->AvatarActor.Get() : nullptr);
    if (Char)
    {
        const FVector Forward = Char->GetActorForwardVector();
        Char->LaunchCharacter(Forward * DashStrength, true, true);
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}