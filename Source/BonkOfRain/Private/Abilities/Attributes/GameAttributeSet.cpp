// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Attributes/GameAttributeSet.h"

#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemComponent.h"

#include "Abilities/System/GameGameplayTags.h"

UGameAttributeSet::UGameAttributeSet()
{
	Health = 100.f;
	MaxHealth = 100.f;
	MoveSpeed = 600.f;
	IncomingDamage = 0.f;
}

void UGameAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGameAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGameAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGameAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGameAttributeSet, IncomingDamage, COND_None, REPNOTIFY_Always);
}

void UGameAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = ClampHealth(NewValue, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
	else if (Attribute == GetMoveSpeedAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}

void UGameAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	UAbilitySystemComponent* TargetASC = Data.Target.AbilityActorInfo.IsValid()
		? Data.Target.AbilityActorInfo->AbilitySystemComponent.Get()
		: nullptr;

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalIncomingDamage = GetIncomingDamage();
		SetIncomingDamage(0.f);

		if (LocalIncomingDamage > 0.f)
		{
			const float NewHealth = ClampHealth(GetHealth() - LocalIncomingDamage, GetMaxHealth());
			SetHealth(NewHealth);

			if (TargetASC && NewHealth <= 0.f)
			{
				TargetASC->AddLooseGameplayTag(GameGameplayTags::Status_Dead);
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(ClampHealth(GetHealth(), GetMaxHealth()));

		if (TargetASC && GetHealth() <= 0.f)
		{
			TargetASC->AddLooseGameplayTag(GameGameplayTags::Status_Dead);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		SetMaxHealth(FMath::Max(GetMaxHealth(), 1.f));
		SetHealth(ClampHealth(GetHealth(), GetMaxHealth()));
	}
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

void UGameAttributeSet::OnRep_IncomingDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGameAttributeSet, IncomingDamage, OldValue);
}

float UGameAttributeSet::ClampHealth(float InHealth, float InMaxHealth)
{
	return FMath::Clamp(InHealth, 0.f, FMath::Max(InMaxHealth, 1.f));
}