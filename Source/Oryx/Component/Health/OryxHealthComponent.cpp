#include "OryxHealthComponent.h"
#include "Component/Stats/OryxStatsComponent.h"

UOryxHealthComponent::UOryxHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UOryxHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Players carry a StatsComponent; enemies don't. If present, drive MaxHealth from stats.
	if (AActor* OwnerActor = GetOwner())
	{
		StatsComp = OwnerActor->FindComponentByClass<UOryxStatsComponent>();
		if (StatsComp)
		{
			StatsComp->OnStatsChanged.AddDynamic(this, &UOryxHealthComponent::HandleStatsChanged);
		}
	}

	CurrentHealth = GetMaxHealth();
}

float UOryxHealthComponent::GetMaxHealth() const
{
	return StatsComp ? StatsComp->GetStat(EOryxStat::MaxHealth) : MaxHealth;
}

void UOryxHealthComponent::HandleStatsChanged()
{
	const float NewMax = GetMaxHealth();
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, NewMax);
	// Broadcast so HUD repaints with new max
	OnDamaged.Broadcast(CurrentHealth, 0.f);
}

void UOryxHealthComponent::ApplyDamageEvent(FOryxDamageEvent Event)
{
	if (Event.BaseAmount <= 0.f || bIsDead) return;

	// TODO(Phase1.W1.Step3): fold damage formula here — type scaling, crit roll, armor mitigation.
	const float FinalDamage = Event.BaseAmount;

	const float Max = GetMaxHealth();
	CurrentHealth = FMath::Clamp(CurrentHealth - FinalDamage, 0.f, Max);

	UE_LOG(LogTemp, Log,
		TEXT("%s took %.1f damage. Health: %.1f / %.1f"),
		*GetOwner()->GetName(), FinalDamage, CurrentHealth, Max);

	OnDamaged.Broadcast(CurrentHealth, FinalDamage);

	if (CurrentHealth <= 0.f)
	{
		bIsDead = true;
		OnDeath.Broadcast(GetOwner());
	}
}

void UOryxHealthComponent::ApplyDamage(float DamageAmount)
{
	FOryxDamageEvent Event;
	Event.BaseAmount = DamageAmount;
	Event.DamageType = EOryxDamageType::Physical;
	ApplyDamageEvent(Event);
}

void UOryxHealthComponent::ApplyHealing(float HealAmount)
{
	if (HealAmount <= 0.f || bIsDead) return;

	const float Max = GetMaxHealth();
	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.f, Max);

	UE_LOG(LogTemp, Log,
		TEXT("%s healed %.1f. Health: %.1f / %.1f"),
		*GetOwner()->GetName(), HealAmount, CurrentHealth, Max);

	OnDamaged.Broadcast(CurrentHealth, -HealAmount);
}

void UOryxHealthComponent::IncreaseMaxHealth(float Amount)
{
	if (Amount <= 0.f || bIsDead) return;

	if (StatsComp)
	{
		// Push an additive modifier on the Stats component. Clamp/broadcast
		// is handled in HandleStatsChanged, which fires on the modifier add.
		FOryxStatModifier Mod;
		Mod.Stat = EOryxStat::MaxHealth;
		Mod.Op = EOryxModOp::Additive;
		Mod.Value = Amount;
		StatsComp->AddModifier(Mod);
	}
	else
	{
		// Fallback path for owners without Stats (enemies).
		MaxHealth += Amount;
		CurrentHealth = MaxHealth;
		OnDamaged.Broadcast(CurrentHealth, -Amount);
	}
}
