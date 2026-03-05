// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Character/GameCharacter.h"

#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

#include "AbilitySystemComponent.h"

#include "Core/PlayerState/GamePlayerState.h"
#include "Abilities/Attributes/GameAttributeSet.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "Abilities/Abilities/GA_Dash.h"

AGameCharacter::AGameCharacter()
{
    bReplicates = true;
    SetReplicateMovement(true);
    PrimaryActorTick.bCanEverTick = false;
}

void AGameCharacter::BeginPlay()
{
    Super::BeginPlay();
    PrintNetState(TEXT("BeginPlay"));
}

void AGameCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    InitializeAbilitySystem();
    PrintNetState(TEXT("PossessedBy"));
}

void AGameCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
    InitializeAbilitySystem();
    PrintNetState(TEXT("OnRep_PlayerState"));
}

void AGameCharacter::InitializeAbilitySystem()
{
    AGamePlayerState* GPS = GetPlayerState<AGamePlayerState>();
    if (!GPS)
    {
        return;
    }

    UAbilitySystemComponent* ASC = GPS->GetAbilitySystemComponent();
    if (!ASC)
    {
        return;
    }

    AbilitySystemComponent = ASC;
    AttributeSet = GPS->GetAttributeSet();
    AbilitySystemComponent->InitAbilityActorInfo(GPS, this);

    if (HasAuthority())
    {
        GiveStartupAbilities();
        ApplyStartupEffects();
    }
}

void AGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAction(TEXT("Debug_Increment"), IE_Pressed, this, &AGameCharacter::IncrementDebugCounter);
    PlayerInputComponent->BindAction(TEXT("Ability_Dash"), IE_Pressed, this, &AGameCharacter::TryActivateDash);
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
    OnRep_DebugCounter();
}

void AGameCharacter::OnRep_DebugCounter()
{
    const FString Msg = FString::Printf(
        TEXT("DebugCounter=%d | LocalRole=%s | LocallyControlled=%s"),
        DebugCounter,
        *UEnum::GetValueAsString(GetLocalRole()),
        IsLocallyControlled() ? TEXT("true") : TEXT("false")
    );

    UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);

    if (GEngine)
    {
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

void AGameCharacter::GiveStartupAbilities()
{
    if (bStartupAbilitiesGiven || !AbilitySystemComponent) return;
    if (!HasAuthority()) return;

    for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartupAbilities)
    {
        if (!AbilityClass) continue;

        FGameplayAbilitySpec Spec(AbilityClass, 1);
        AbilitySystemComponent->GiveAbility(Spec);
    }

    bStartupAbilitiesGiven = true;
    UE_LOG(LogTemp, Warning, TEXT("StartupAbilities count: %d"), StartupAbilities.Num());
}

void AGameCharacter::ApplyStartupEffects()
{
    if (bStartupEffectsApplied || !AbilitySystemComponent) return;
    if (!HasAuthority()) return;

    if (StartupAttributesEffect)
    {
        const FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
        const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(StartupAttributesEffect, 1.0f, Context);

        if (SpecHandle.IsValid())
        {
            AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        }
    }

    bStartupEffectsApplied = true;
}
void AGameCharacter::TryActivateDash()
{
    if (!AbilitySystemComponent) return;

    AbilitySystemComponent->TryActivateAbilityByClass(UGA_Dash::StaticClass());
}