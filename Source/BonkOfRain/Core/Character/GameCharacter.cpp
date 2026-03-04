// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCharacter.h"

#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerState.h"

AGameCharacter::AGameCharacter()
{
	bReplicates = true;
    SetReplicateMovement(true);
	PrimaryActorTick.bCanEverTick = true;
}

void AGameCharacter::BeginPlay()
{
	Super::BeginPlay();
	PrintNetState(TEXT("Beginplay"));
}

void AGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction(TEXT("Debug_Increment"), IE_Pressed, this, &AGameCharacter::IncrementDebugCounter);
}

void AGameCharacter::IncrementDebugCounter()
{
	if (HasAuthority())
	{
		DebugCounter++;
		OnRep_DebugCounter();

	}
	else
	{
		Server_IncrementCounter();
	}
}

void AGameCharacter::Server_IncrementCounter_Implementation()
{
    DebugCounter++;
    // Сервер сам изменил — можно вызвать для вывода на экран сервера
    OnRep_DebugCounter();
}

void AGameCharacter::OnRep_DebugCounter()
{
    const FString Msg = FString::Printf(
        TEXT("DebugCounter = %d | LocalRole=%s | LocallyControlled=%s"),
        DebugCounter,
        *UEnum::GetValueAsString(GetLocalRole()),
        IsLocallyControlled() ? TEXT("true") : TEXT("false")
    );

    UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);

    if (GEngine)
    {
        // Разный ключ чтобы сообщения обновлялись, а не спамились бесконечно
        const int32 Key = (IsLocallyControlled() ? 1001 : 1002) + (HasAuthority() ? 10 : 0);
        GEngine->AddOnScreenDebugMessage(Key, 2.0f, FColor::Yellow, Msg);
    }
}

void AGameCharacter::PrintNetState(const FString& Context) const
{
    const ENetMode NM = GetNetMode();

    const FString Msg = FString::Printf(
        TEXT("[%s] NetMode=%d | HasAuth=%s | LocalRole=%s | RemoteRole=%s | LocallyControlled=%s"),
        *Context,
        (int32)NM,
        HasAuthority() ? TEXT("true") : TEXT("false"),
        *UEnum::GetValueAsString(GetLocalRole()),
        *UEnum::GetValueAsString(GetRemoteRole()),
        IsLocallyControlled() ? TEXT("true") : TEXT("false")
    );

    UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);

    if (GEngine)
    {
        const int32 Key = IsLocallyControlled() ? 9001 : 9002;
        GEngine->AddOnScreenDebugMessage(Key, 5.0f, FColor::Cyan, Msg);
    }
}

void AGameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AGameCharacter, DebugCounter);
} 