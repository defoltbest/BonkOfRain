// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Attributes/GameAttributeSet.h"
#include "Net/UnrealNetwork.h"

UGameAttributeSet::UGameAttributeSet()
{
    Health = 100.f;
    MaxHealth = 100.f;
    MoveSpeed = 600.f;
}

void UGameAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION_NOTIFY(UGameAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UGameAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UGameAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
}

void UGameAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGameAttributeSet, Health, OldValue);
}

void UGameAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGameAttributeSet, MaxHealth, OldValue);
}

void UGameAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGameAttributeSet, MoveSpeed, OldValue);
}