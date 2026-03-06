// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Character/GameCharacter.h"

#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

#include "GameFramework/PlayerState.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"

#include "Core/PlayerState/GamePlayerState.h"
#include "Abilities/Attributes/GameAttributeSet.h"

AGameCharacter::AGameCharacter()
{
	bReplicates = true;
	SetReplicateMovement(true);

	PrimaryActorTick.bCanEverTick = true;
}

void AGameCharacter::BeginPlay()
{
	Super::BeginPlay();

}

UAbilitySystemComponent* AGameCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AGameCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitializeAbilitySystem();
}

void AGameCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitializeAbilitySystem();
}

void AGameCharacter::InitializeAbilitySystem()
{
	AGamePlayerState* PS = GetPlayerState<AGamePlayerState>();
	if (!PS)
	{
		UE_LOG(LogTemp, Warning, TEXT("InitAS: NO PlayerState yet"));
		return;
	}

	AbilitySystemComponent = PS->GetAbilitySystemComponent();
	AttributeSet = PS->GetAttributeSet();

	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("InitAS: NO ASC on PlayerState"));
		return;
	}

	AbilitySystemComponent->InitAbilityActorInfo(PS, this);

	UE_LOG(LogTemp, Warning, TEXT("InitAS: OK | HasAuth=%d"), HasAuthority() ? 1 : 0);

	if (HasAuthority())
	{
		GiveStartupAbilities();
		ApplyStartupEffect();
	}
}

void AGameCharacter::GiveStartupAbilities()
{
	if (bStartupGranted)
	{
		return;
	}

	if (!AbilitySystemComponent)
	{
		return;
	}

	if (StartupAbilities.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartupAbilities count: 0"));
		bStartupGranted = true;
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("StartupAbilities count: %d"), StartupAbilities.Num());

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartupAbilities)
	{
		if (!AbilityClass)
		{
			continue;
		}

		FGameplayAbilitySpec Spec(AbilityClass, 1);
		AbilitySystemComponent->GiveAbility(Spec);
	}

	// важно: после GiveAbility — пушим изменения клиентам
	AbilitySystemComponent->ForceReplication();

	bStartupGranted = true;
}

void AGameCharacter::ApplyStartupEffect()
{
	if (!AbilitySystemComponent || !StartupAttributesEffect)
	{
		return;
	}


	FGameplayEffectContextHandle Ctx = AbilitySystemComponent->MakeEffectContext();
	Ctx.AddSourceObject(this);

	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(StartupAttributesEffect, 1.f, Ctx);
	if (SpecHandle.IsValid())
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Debug_Increment"), IE_Pressed, this, &AGameCharacter::IncrementDebugCounter);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (DashAction)
		{
			EIC->BindAction(DashAction, ETriggerEvent::Triggered, this, &AGameCharacter::OnDashPressed);
		}
	}
}

void AGameCharacter::OnDashPressed()
{
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dash pressed but ASC is null"));
		return;
	}

	if (StartupAbilities.Num() <= 0 || !StartupAbilities[0])
	{
		UE_LOG(LogTemp, Warning, TEXT("Dash pressed but StartupAbilities[0] is empty"));
		return;
	}

	AbilitySystemComponent->TryActivateAbilityByClass(StartupAbilities[0]);
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

void AGameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGameCharacter, DebugCounter);
}