// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayEffectTypes.h" 
#include "GameCharacter.generated.h"

class UAbilitySystemComponent;
class UGameAttributeSet;

UCLASS()
class BONKOFRAIN_API AGameCharacter : public ACharacter
{
    GENERATED_BODY()

public:

    AGameCharacter();

protected:

    UPROPERTY(EditDefaultsOnly, Category = "GAS|Startup")
    TArray<TSubclassOf<class UGameplayAbility>> StartupAbilities;

    UPROPERTY(EditDefaultsOnly, Category = "GAS|Startup")
    TSubclassOf<class UGameplayEffect> StartupAttributesEffect;

    bool bStartupAbilitiesGiven = false;
    bool bStartupEffectsApplied = false;

    void GiveStartupAbilities();
    void ApplyStartupEffects();

    virtual void BeginPlay() override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual void PossessedBy(AController* NewController) override;

    virtual void OnRep_PlayerState() override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


private:

    void TryActivateDash();

    void InitializeAbilitySystem();

    void IncrementDebugCounter();

    void PrintNetState(const FString& Context) const;


    UFUNCTION(Server, Reliable)
    void Server_IncrementCounter();


    UFUNCTION()
    void OnRep_DebugCounter();


    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;


    UPROPERTY()
    TObjectPtr<UGameAttributeSet> AttributeSet;


    UPROPERTY(ReplicatedUsing = OnRep_DebugCounter)
    int32 DebugCounter = 0;
};