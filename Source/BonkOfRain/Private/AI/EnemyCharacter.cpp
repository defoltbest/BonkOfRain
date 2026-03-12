// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnemyCharacter.h"
#include "AI/EnemyAIController.h"
#include "AbilitySystemGlobals.h"
#include "AIController.h"
#include "AbilitySystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

#include "Core/Character/GameCharacter.h"
#include "Combat/GameDamageLibrary.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	CachedAIController = Cast<AAIController>(GetController());
}

void AEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!CachedAIController)
	{
		CachedAIController = Cast<AAIController>(GetController());
	}

	UpdateTarget();

	if (!CurrentTarget)
	{
		return;
	}

	if (bDrawDebug)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), AttackRange, 16, FColor::Red, false, 0.f);
	}

	if (CanAttackTarget())
	{
		TryAttackTarget();
	}
	else
	{
		MoveToTarget();
	}
}

void AEnemyCharacter::UpdateTarget()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn)
	{
		CurrentTarget = nullptr;
		return;
	}

	CurrentTarget = Cast<AGameCharacter>(PlayerPawn);
}

void AEnemyCharacter::MoveToTarget()
{
	if (!CachedAIController || !CurrentTarget)
	{
		return;
	}

	CachedAIController->MoveToActor(CurrentTarget, AttackRange * 0.8f);
}

bool AEnemyCharacter::CanAttackTarget() const
{
	if (!CurrentTarget)
	{
		return false;
	}

	if (CurrentTarget->IsDead())
	{
		return false;
	}

	const float TimeNow = GetWorld()->GetTimeSeconds();
	const bool bCooldownReady = TimeNow >= LastAttackTime + AttackCooldown;
	const bool bInRange = GetDistanceToTarget() <= AttackRange;

	return bCooldownReady && bInRange;
}

float AEnemyCharacter::GetDistanceToTarget() const
{
	if (!CurrentTarget)
	{
		return TNumericLimits<float>::Max();
	}

	return FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());
}

void AEnemyCharacter::TryAttackTarget()
{
	if (!CurrentTarget || !DamageEffectClass)
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = CurrentTarget->GetAbilitySystemComponent();
	UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(this);

	// ѕока у врага ASC нет, так что source может быть null.
	// ƒл€ первого шага позволим использовать target ASC как source fallback.
	if (!SourceASC)
	{
		SourceASC = TargetASC;
	}

	if (!TargetASC || !SourceASC)
	{
		return;
	}

	const bool bApplied = UGameDamageLibrary::ApplyDamageEffect(SourceASC, TargetASC, DamageEffectClass, 1.f);

	if (bApplied)
	{
		LastAttackTime = GetWorld()->GetTimeSeconds();

		UE_LOG(LogTemp, Warning, TEXT("Enemy %s attacked %s"), *GetName(), *CurrentTarget->GetName());
	}
}

