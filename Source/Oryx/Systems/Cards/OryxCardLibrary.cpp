#include "OryxCardLibrary.h"
#include "Component/Stats/OryxStatsComponent.h"

namespace
{
	// Helper: build a card with a single Additive modifier in one line.
	static FOryxCard MakeAddCard(FName Id, EOryxCardCategory Cat, const FText& Name, const FText& Desc,
		EOryxStat Stat, float Value)
	{
		FOryxCard Card;
		Card.CardId = Id;
		Card.Category = Cat;
		Card.DisplayName = Name;
		Card.Description = Desc;
		Card.Modifier.Stat = Stat;
		Card.Modifier.Op = EOryxModOp::Additive;
		Card.Modifier.Value = Value;
		// SourceId left empty — generated at apply time by StatsComponent.AddModifier.
		return Card;
	}

	static FOryxCard MakeMulCard(FName Id, EOryxCardCategory Cat, const FText& Name, const FText& Desc,
		EOryxStat Stat, float Value)
	{
		FOryxCard Card;
		Card.CardId = Id;
		Card.Category = Cat;
		Card.DisplayName = Name;
		Card.Description = Desc;
		Card.Modifier.Stat = Stat;
		Card.Modifier.Op = EOryxModOp::Multiplicative;
		Card.Modifier.Value = Value;
		return Card;
	}

	// --- Strength pool ---
	// PhysicalPower, MaxHealth, Armor, Crit Chance per D15.
	// "Damage when low HP" deferred — needs HealthComponent threshold hook (post-MVP behavior change per D7).
	static TArray<FOryxCard> BuildStrengthPool()
	{
		return {
			MakeAddCard(TEXT("Card_Str_PhysPower"), EOryxCardCategory::Strength,
				NSLOCTEXT("OryxCards", "PhysPower_Name", "Crushing Form"),
				NSLOCTEXT("OryxCards", "PhysPower_Desc", "+15% Physical Power"),
				EOryxStat::PhysicalPower, 0.15f),

			MakeAddCard(TEXT("Card_Str_MaxHealth"), EOryxCardCategory::Strength,
				NSLOCTEXT("OryxCards", "MaxHP_Name", "Iron Heart"),
				NSLOCTEXT("OryxCards", "MaxHP_Desc", "+25 Max Health"),
				EOryxStat::MaxHealth, 25.f),

			MakeAddCard(TEXT("Card_Str_Armor"), EOryxCardCategory::Strength,
				NSLOCTEXT("OryxCards", "Armor_Name", "Bulwark"),
				NSLOCTEXT("OryxCards", "Armor_Desc", "+15 Armor"),
				EOryxStat::Armor, 15.f),

			MakeAddCard(TEXT("Card_Str_CritChance"), EOryxCardCategory::Strength,
				NSLOCTEXT("OryxCards", "CritStr_Name", "Killing Edge"),
				NSLOCTEXT("OryxCards", "CritStr_Desc", "+5% Critical Chance"),
				EOryxStat::CritChance, 0.05f)
		};
	}

	// --- Magic pool ---
	// Fire/Cold/Magic/Curse/Poison damage, MaxMana, ManaRegen, CastSpeed, CDR per D15.
	static TArray<FOryxCard> BuildMagicPool()
	{
		return {
			MakeAddCard(TEXT("Card_Mag_Fire"), EOryxCardCategory::Magic,
				NSLOCTEXT("OryxCards", "Fire_Name", "Ember Touch"),
				NSLOCTEXT("OryxCards", "Fire_Desc", "+15% Fire Damage"),
				EOryxStat::FireDamage, 0.15f),

			MakeAddCard(TEXT("Card_Mag_Cold"), EOryxCardCategory::Magic,
				NSLOCTEXT("OryxCards", "Cold_Name", "Frost Veil"),
				NSLOCTEXT("OryxCards", "Cold_Desc", "+15% Cold Damage"),
				EOryxStat::ColdDamage, 0.15f),

			MakeAddCard(TEXT("Card_Mag_Magic"), EOryxCardCategory::Magic,
				NSLOCTEXT("OryxCards", "Mag_Name", "Arcane Lens"),
				NSLOCTEXT("OryxCards", "Mag_Desc", "+15% Magic Damage"),
				EOryxStat::MagicDamage, 0.15f),

			MakeAddCard(TEXT("Card_Mag_Curse"), EOryxCardCategory::Magic,
				NSLOCTEXT("OryxCards", "Curse_Name", "Hex Sign"),
				NSLOCTEXT("OryxCards", "Curse_Desc", "+15% Curse Damage"),
				EOryxStat::CurseDamage, 0.15f),

			MakeAddCard(TEXT("Card_Mag_Poison"), EOryxCardCategory::Magic,
				NSLOCTEXT("OryxCards", "Poison_Name", "Venom Brand"),
				NSLOCTEXT("OryxCards", "Poison_Desc", "+15% Poison Damage"),
				EOryxStat::PoisonDamage, 0.15f),

			MakeAddCard(TEXT("Card_Mag_MaxMana"), EOryxCardCategory::Magic,
				NSLOCTEXT("OryxCards", "MaxMana_Name", "Wellspring"),
				NSLOCTEXT("OryxCards", "MaxMana_Desc", "+25 Max Mana"),
				EOryxStat::MaxMana, 25.f),

			MakeAddCard(TEXT("Card_Mag_ManaRegen"), EOryxCardCategory::Magic,
				NSLOCTEXT("OryxCards", "ManaRegen_Name", "Replenish"),
				NSLOCTEXT("OryxCards", "ManaRegen_Desc", "+5 Mana Regen"),
				EOryxStat::ManaRegen, 5.f),

			MakeMulCard(TEXT("Card_Mag_CastSpeed"), EOryxCardCategory::Magic,
				NSLOCTEXT("OryxCards", "CastSpeed_Name", "Quickcast"),
				NSLOCTEXT("OryxCards", "CastSpeed_Desc", "+10% Cast Speed"),
				EOryxStat::CastSpeed, 0.10f),

			MakeAddCard(TEXT("Card_Mag_CDR"), EOryxCardCategory::Magic,
				NSLOCTEXT("OryxCards", "CDR_Name", "Focus"),
				NSLOCTEXT("OryxCards", "CDR_Desc", "+5% Cooldown Reduction"),
				EOryxStat::CooldownReduction, 0.05f)
		};
	}

	// --- Agility pool ---
	// CritChance (also in Strength), CritDamage, AttackSpeed, MovementSpeed, SprintRegen per D15.
	// SprintRegen → StaminaRegen (same stat post stat-enum expansion 2026-05-10).
	static TArray<FOryxCard> BuildAgilityPool()
	{
		return {
			MakeAddCard(TEXT("Card_Agi_CritChance"), EOryxCardCategory::Agility,
				NSLOCTEXT("OryxCards", "CritAgi_Name", "Precision"),
				NSLOCTEXT("OryxCards", "CritAgi_Desc", "+5% Critical Chance"),
				EOryxStat::CritChance, 0.05f),

			MakeAddCard(TEXT("Card_Agi_CritDamage"), EOryxCardCategory::Agility,
				NSLOCTEXT("OryxCards", "CritDmg_Name", "Deep Strike"),
				NSLOCTEXT("OryxCards", "CritDmg_Desc", "+10% Critical Damage"),
				EOryxStat::CritDamage, 0.10f),

			MakeMulCard(TEXT("Card_Agi_AttackSpeed"), EOryxCardCategory::Agility,
				NSLOCTEXT("OryxCards", "AS_Name", "Swift Hands"),
				NSLOCTEXT("OryxCards", "AS_Desc", "+10% Attack Speed"),
				EOryxStat::AttackSpeed, 0.10f),

			MakeMulCard(TEXT("Card_Agi_MoveSpeed"), EOryxCardCategory::Agility,
				NSLOCTEXT("OryxCards", "MS_Name", "Wind Step"),
				NSLOCTEXT("OryxCards", "MS_Desc", "+10% Movement Speed"),
				EOryxStat::MovementSpeed, 0.10f),

			MakeAddCard(TEXT("Card_Agi_StaminaRegen"), EOryxCardCategory::Agility,
				NSLOCTEXT("OryxCards", "Stamina_Name", "Second Wind"),
				NSLOCTEXT("OryxCards", "Stamina_Desc", "+10 Stamina Regen"),
				EOryxStat::StaminaRegen, 10.f)
		};
	}
}

TArray<FOryxCard> UOryxCardLibrary::GetCardsForCategory(EOryxCardCategory Category)
{
	switch (Category)
	{
		case EOryxCardCategory::Strength: return BuildStrengthPool();
		case EOryxCardCategory::Magic:    return BuildMagicPool();
		case EOryxCardCategory::Agility:  return BuildAgilityPool();
		default:                          return {};
	}
}

TArray<FOryxCard> UOryxCardLibrary::RollCards()
{
	TArray<FOryxCard> Result;
	Result.Reserve(3);

	const EOryxCardCategory Categories[] = {
		EOryxCardCategory::Strength,
		EOryxCardCategory::Magic,
		EOryxCardCategory::Agility
	};

	for (EOryxCardCategory Cat : Categories)
	{
		TArray<FOryxCard> Pool = GetCardsForCategory(Cat);
		if (Pool.Num() == 0) continue;
		const int32 Idx = FMath::RandRange(0, Pool.Num() - 1);
		Result.Add(Pool[Idx]);
	}

	return Result;
}

FGuid UOryxCardLibrary::ApplyCardToStats(const FOryxCard& Card, UOryxStatsComponent* Stats)
{
	if (!Stats) return FGuid();
	// StatsComponent.AddModifier auto-generates a fresh GUID since the card pool's modifier
	// has an empty SourceId.
	return Stats->AddModifier(Card.Modifier);
}
