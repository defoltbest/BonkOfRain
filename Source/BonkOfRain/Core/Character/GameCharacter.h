// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameCharacter.generated.h"

UCLASS()
class BONKOFRAIN_API AGameCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGameCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;//replication

	UPROPERTY(ReplicatedUsing = OnRep_DebugCounter)
	int32 DebugCounter = 0;

	UFUNCTION()
	void OnRep_DebugCounter();

	UFUNCTION(Server, Reliable)
	void Server_IncrementCounter();

private:
	void IncrementDebugCounter();
	void PrintNetState(const FString& Context) const;

};
