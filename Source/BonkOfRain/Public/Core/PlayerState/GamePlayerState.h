// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GamePlayerState.generated.h"

class UGameAbilitySystemComponent;
class UGameAttributeSet;

UCLASS()
class BONKOFRAIN_API AGamePlayerState : public APlayerState, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AGamePlayerState();

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    UGameAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
    TObjectPtr<UGameAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<UGameAttributeSet> AttributeSet;
};