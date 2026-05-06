#include "OryxHealthComponent.h"
#include "Component/Stats/OryxStatsComponent.h"
#include "GameFramework/Actor.h"

namespace
{
	/** Returns the elemental %-bonus stat for the type, or nullopt for Physical (which scales via PhysicalPower instead). */
	bool TryGetElementalStat(EOryxDamageType Type, EOryxStat& OutStat)
	{
		switch (Type)
		{
		case EOryxDamageType::Fire:   OutStat = EOryxStat::FireDamage;   return true;
		case EOryxDamageType::Cold:   OutStat = EOryxStat::ColdDamage;   return true;
		case EOryxDamageType::Magic:  OutStat = EOryxStat::MagicDamage;  return true;
		case EOryxDamageType::Curse:  OutStat = EOryxStat::CurseDamage;  return true;
		case EOryxDamageType::Poison: OutStat = EOryxStat::PoisonDamage; return true;
		default: return false;
		}
	}

	UOryxStatsComponent* FindStats(const AActor* Actor)
	{
		return Actor ? Actor->FindComponentByClass<UOryxStatsComponent>() : nullptr;
	}
}

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

	// --- Formula (D1) ---
	// Final = Base × (1 + Stats[DamageType])
	//             × (1 + Stats[PhysicalPower] if Physical)
	//             × CritMult
	//             × max(0, 1 - Stats[Armor]/100)
	float Damage = Event.BaseAmount;
	bool bWasCrit = false;

	if (UOryxStatsComponent* InstStats = FindStats(Event.Instigator.Get()))
	{
		// Type bonus: elemental types use their %-bonus stat; physical uses PhysicalPower.
		EOryxStat TypeStat;
		if (TryGetElementalStat(Event.DamageType, TypeStat))
		{
			Damage *= (1.f + InstStats->GetStat(TypeStat));
		}
		else // Physical
		{
			Damage *= (1.f + InstStats->GetStat(EOryxStat::PhysicalPower));
		}

		const float CritChance = InstStats->GetStat(EOryxStat::CritChance);
		if (CritChance > 0.f && FMath::FRand() < CritChance)
		{
			const float CritMult = InstStats->GetStat(EOryxStat::CritDamage);
			// CritDamage stat is the multiplier itself (e.g. 1.5 = 150%). Guard against 0.
			Damage *= (CritMult > 0.f ? CritMult : 1.f);
			bWasCrit = true;
		}
	}

	if (UOryxStatsComponent* TargetStats = FindStats(GetOwner()))
	{
		const float Armor = TargetStats->GetStat(EOryxStat::Armor);
		Damage *= FMath::Max(0.f, 1.f - Armor / 100.f);
	}

	const float FinalDamage = Damage;
	if (FinalDamage <= 0.f) return;

	const float Max = GetMaxHealth();
	CurrentHealth = FMath::Clamp(CurrentHealth - FinalDamage, 0.f, Max);

	UE_LOG(LogTemp, Log,
		TEXT("%s took %.1f damage. Health: %.1f / %.1f"),
		*GetOwner()->GetName(), FinalDamage, CurrentHealth, Max);

	OnDamaged.Broadcast(CurrentHealth, FinalDamage);
	OnDamageReceived.Broadcast(Event, FinalDamage, bWasCrit);

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
