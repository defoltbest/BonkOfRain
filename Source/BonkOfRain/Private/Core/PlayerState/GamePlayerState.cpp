// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PlayerState/GamePlayerState.h"
#include "Abilities/System/GameAbilitySystemComponent.h"
#include "Abilities/Attributes/GameAttributeSet.h"


AGamePlayerState::AGamePlayerState()
{
    AbilitySystemComponent = CreateDefaultSubobject<UGameAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
    AttributeSet = CreateDefaultSubobject<UGameAttributeSet>(TEXT("AttributeSet"));
    bReplicates = true;
}

UAbilitySystemComponent* AGamePlayerState::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}