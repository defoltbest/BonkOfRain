// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Character/GamePlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

void AGamePlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (ULocalPlayer* LP = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsys = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (DefaultMappingContext)
            {
                Subsys->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }
}