// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/GameDamageLibrary.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

bool UGameDamageLibrary::ApplyDamageEffect(
	UAbilitySystemComponent* SourceASC,
	UAbilitySystemComponent* TargetASC,
	TSubclassOf<UGameplayEffect> DamageEffectClass,
	float EffectLevel)
{
	if (!SourceASC || !TargetASC || !DamageEffectClass)
	{
		return false;
	}

	FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(SourceASC->GetAvatarActor());

	const FGameplayEffectSpecHandle SpecHandle =
		SourceASC->MakeOutgoingSpec(DamageEffectClass, EffectLevel, EffectContext);

	if (!SpecHandle.IsValid())
	{
		return false;
	}

	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	return true;
}