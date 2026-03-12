// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameDamageLibrary.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;

UCLASS()
class BONKOFRAIN_API UGameDamageLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|Damage")
	static bool ApplyDamageEffect(
		UAbilitySystemComponent* SourceASC,
		UAbilitySystemComponent* TargetASC,
		TSubclassOf<UGameplayEffect> DamageEffectClass,
		float EffectLevel = 1.0f);
};