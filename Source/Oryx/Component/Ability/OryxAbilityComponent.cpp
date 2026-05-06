#include "OryxAbilityComponent.h"
#include "Systems/Abilities/Base/OryxAbility.h"
#include "Component/Stats/OryxStatsComponent.h"
#include "Engine/World.h"

UOryxAbilityComponent::UOryxAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UOryxAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	Abilities.Reserve(AbilityClasses.Num());
	for (TSubclassOf<UOryxAbility> AbilityClass : AbilityClasses)
	{
		if (!AbilityClass) continue;
		UOryxAbility* Ability = NewObject<UOryxAbility>(GetOwner(), AbilityClass);
		Abilities.Add(Ability);
	}
}

bool UOryxAbilityComponent::TryActivateAbility(int32 Index)
{
	if (!Abilities.IsValidIndex(Index)) return false;

	UOryxAbility* Ability = Abilities[Index];
	if (!Ability) return false;

	const UWorld* World = GetWorld();
	if (!World) return false;

	const float Now = World->GetTimeSeconds();

	// Stat-scaled cooldown: basic attacks scale with AttackSpeed; everything scales with CooldownReduction.
	const UOryxStatsComponent* Stats = GetOwner() ? GetOwner()->FindComponentByClass<UOryxStatsComponent>() : nullptr;
	const float Effective = Ability->GetEffectiveCooldown(Stats);
	if ((Now - Ability->GetLastActivatedTime()) < Effective) return false;

	Ability->Activate(GetOwner());
	Ability->MarkActivated(Now);
	return true;
}

UOryxAbility* UOryxAbilityComponent::GetAbility(int32 Index) const
{
	return Abilities.IsValidIndex(Index) ? Abilities[Index] : nullptr;
}
