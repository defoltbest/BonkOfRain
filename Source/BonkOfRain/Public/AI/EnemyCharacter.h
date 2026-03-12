// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class UGameplayEffect;
class AAIController;
class AGameCharacter;

UCLASS()
class BONKOFRAIN_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void UpdateTarget();
	void MoveToTarget();
	void TryAttackTarget();

	bool CanAttackTarget() const;
	float GetDistanceToTarget() const;

protected:
	UPROPERTY(EditAnywhere, Category = "Enemy|Combat")
	float AttackRange = 200.f;

	UPROPERTY(EditAnywhere, Category = "Enemy|Combat")
	float AttackCooldown = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Enemy|Combat")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditAnywhere, Category = "Enemy|Debug")
	bool bDrawDebug = true;

private:
	UPROPERTY()
	TObjectPtr<AAIController> CachedAIController = nullptr;

	UPROPERTY()
	TObjectPtr<AGameCharacter> CurrentTarget = nullptr;

	float LastAttackTime = -1000.f;
};