// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/System/GameAbilitySystemComponent.h"

bool UGameAbilitySystemComponent::TryActivateAbilityByInputTag(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return false;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.Ability && AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			return TryActivateAbility(AbilitySpec.Handle);
		}
	}

	return false;
}