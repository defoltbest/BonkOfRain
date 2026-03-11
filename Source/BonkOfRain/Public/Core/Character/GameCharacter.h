// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameCharacter.generated.h"

class UAbilitySystemComponent;
class UGameAbilitySystemComponent;
class UGameAttributeSet;
class UInputAction;
class UGameplayAbility;
class UGameplayEffect;

UCLASS()
class BONKOFRAIN_API AGameCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGameCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	const UGameAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void InitializeAbilitySystem();
	void GiveStartupAbilities();
	void ApplyStartupEffect();

	void OnDashPressed();
	void IncrementDebugCounter();

	UFUNCTION(Server, Reliable)
	void Server_IncrementCounter();

	UFUNCTION()
	void OnRep_DebugCounter();

private:
	UPROPERTY(Transient)
	TObjectPtr<UGameAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UGameAttributeSet> AttributeSet = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_DebugCounter)
	int32 DebugCounter = 0;

	UPROPERTY(EditDefaultsOnly, Category = "GAS|Startup")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "GAS|Startup")
	TSubclassOf<UGameplayEffect> StartupAttributesEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> DashAction = nullptr;

	bool bStartupGranted = false;
};