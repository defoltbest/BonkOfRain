// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Character/GameCharacter.h"

#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Core/PlayerState/GamePlayerState.h"
#include "Abilities/Attributes/GameAttributeSet.h"
#include "Abilities/System/GameAbilitySystemComponent.h"
#include "Abilities/System/GameGameplayTags.h"
#include "Abilities/Abilities/GA_Dash.h"

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

void AGameCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bIsDead && IsDead())
	{
		HandleDeath();
	}
}

UAbilitySystemComponent* AGameCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool AGameCharacter::IsDead() const
{
	if (!AbilitySystemComponent)
	{
		return bIsDead;
	}

	return bIsDead || AbilitySystemComponent->HasMatchingGameplayTag(GameGameplayTags::Status_Dead);
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

	AbilitySystemComponent = Cast<UGameAbilitySystemComponent>(PS->GetAbilitySystemComponent());
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
	if (bStartupGranted || !AbilitySystemComponent)
	{
		return;
	}

	if (StartupAbilities.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartupAbilities count: 0"));
		bStartupGranted = true;
		return;
	}

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartupAbilities)
	{
		if (!AbilityClass)
		{
			continue;
		}

		FGameplayAbilitySpec Spec(AbilityClass, 1);

		if (AbilityClass->IsChildOf(UGA_Dash::StaticClass()))
		{
			Spec.DynamicAbilityTags.AddTag(GameGameplayTags::InputTag_Ability_Dash);
		}

		AbilitySystemComponent->GiveAbility(Spec);
	}

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

	const FGameplayEffectSpecHandle SpecHandle =
		AbilitySystemComponent->MakeOutgoingSpec(StartupAttributesEffect, 1.f, Ctx);

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
	if (bIsDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dash blocked: character is dead"));
		return;
	}

	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dash pressed but ASC is null"));
		return;
	}

	const bool bActivated = AbilitySystemComponent->TryActivateAbilityByInputTag(GameGameplayTags::InputTag_Ability_Dash);

	UE_LOG(LogTemp, Warning, TEXT("Dash input pressed | Activated=%s"), bActivated ? TEXT("true") : TEXT("false"));
}

void AGameCharacter::HandleDeath()
{
	bIsDead = true;

	if (Controller)
	{
		DisableInput(Cast<APlayerController>(Controller));
	}

	GetCharacterMovement()->DisableMovement();

	UE_LOG(LogTemp, Warning, TEXT("Character died: %s"), *GetName());

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("%s died"), *GetName()));
	}

	BP_OnDeath();
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
		IsLocallyControlled() ? TEXT("true") : TEXT("false"));

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
	DOREPLIFETIME(AGameCharacter, bIsDead);
}