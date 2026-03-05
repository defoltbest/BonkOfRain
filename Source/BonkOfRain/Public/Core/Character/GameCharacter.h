// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

    virtual void BeginPlay() override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual void PossessedBy(AController* NewController) override;

    virtual void OnRep_PlayerState() override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


private:

    void InitializeAbilitySystem();

    void IncrementDebugCounter();

    void PrintNetState(const FString& Context) const;


    UFUNCTION(Server, Reliable)
    void Server_IncrementCounter();


    UFUNCTION()
    void OnRep_DebugCounter();


private:

    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;


    UPROPERTY()
    TObjectPtr<UGameAttributeSet> AttributeSet;


    UPROPERTY(ReplicatedUsing = OnRep_DebugCounter)
    int32 DebugCounter = 0;
};