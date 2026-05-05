#include "OryxStatsComponent.h"

UOryxStatsComponent::UOryxStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Sensible defaults so a fresh character without BP configuration still functions.
	// BP Class Defaults override these per-class.
	BaseStats.Add(EOryxStat::MaxHealth, 100.f);
	BaseStats.Add(EOryxStat::MovementSpeed, 500.f);
	BaseStats.Add(EOryxStat::Damage, 25.f);
	BaseStats.Add(EOryxStat::AttackSpeed, 1.0f);
	BaseStats.Add(EOryxStat::CritChance, 0.05f);          // 5%
	BaseStats.Add(EOryxStat::CritDamage, 1.5f);           // 150%
	BaseStats.Add(EOryxStat::PhysicalPower, 0.f);
	BaseStats.Add(EOryxStat::FireDamage, 0.f);
	BaseStats.Add(EOryxStat::ColdDamage, 0.f);
	BaseStats.Add(EOryxStat::MagicDamage, 0.f);
	BaseStats.Add(EOryxStat::CurseDamage, 0.f);
	BaseStats.Add(EOryxStat::PoisonDamage, 0.f);
	BaseStats.Add(EOryxStat::Luck, 0.f);
	BaseStats.Add(EOryxStat::CooldownReduction, 0.f);
	BaseStats.Add(EOryxStat::Armor, 0.f);
	BaseStats.Add(EOryxStat::CastSpeed, 1.0f);
	BaseStats.Add(EOryxStat::MaxMana, 100.f);
	BaseStats.Add(EOryxStat::MaxStamina, 100.f);
}

float UOryxStatsComponent::GetBaseStat(EOryxStat Stat) const
{
	const float* Found = BaseStats.Find(Stat);
	return Found ? *Found : 0.f;
}

float UOryxStatsComponent::GetStat(EOryxStat Stat) const
{
	float Base = GetBaseStat(Stat);
	float SumAdditive = 0.f;
	float SumMultiplicative = 0.f;

	for (const FOryxStatModifier& Mod : Modifiers)
	{
		if (Mod.Stat != Stat) continue;
		if (Mod.Op == EOryxModOp::Additive)
		{
			SumAdditive += Mod.Value;
		}
		else
		{
			SumMultiplicative += Mod.Value;
		}
	}

	return (Base + SumAdditive) * (1.f + SumMultiplicative);
}

FGuid UOryxStatsComponent::AddModifier(FOryxStatModifier Modifier)
{
	if (!Modifier.SourceId.IsValid())
	{
		Modifier.SourceId = FGuid::NewGuid();
	}
	Modifiers.Add(Modifier);
	OnStatsChanged.Broadcast();
	return Modifier.SourceId;
}

void UOryxStatsComponent::RemoveModifiersBySource(FGuid SourceId)
{
	if (!SourceId.IsValid()) return;

	const int32 Removed = Modifiers.RemoveAll([&SourceId](const FOryxStatModifier& M)
	{
		return M.SourceId == SourceId;
	});

	if (Removed > 0)
	{
		OnStatsChanged.Broadcast();
	}
}

void UOryxStatsComponent::ClearAllModifiers()
{
	if (Modifiers.IsEmpty()) return;
	Modifiers.Empty();
	OnStatsChanged.Broadcast();
}
