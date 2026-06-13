#include "OryxStatFormatLibrary.h"

bool UOryxStatFormatLibrary::IsPercentageStat(EOryxStat Stat)
{
	switch (Stat)
	{
	// Stored as fractions, shown as %.
	case EOryxStat::Damage:
	case EOryxStat::PhysicalPower:
	case EOryxStat::FireDamage:
	case EOryxStat::ColdDamage:
	case EOryxStat::MagicDamage:
	case EOryxStat::CurseDamage:
	case EOryxStat::PoisonDamage:
	case EOryxStat::CritChance:
	case EOryxStat::CritDamage:
	case EOryxStat::AttackSpeed:
	case EOryxStat::CastSpeed:
	case EOryxStat::CooldownReduction:
	case EOryxStat::DamageReduction:
	case EOryxStat::GoldGain:
		return true;

	// Flat integer stats.
	case EOryxStat::MaxHealth:
	case EOryxStat::MaxMana:
	case EOryxStat::MaxStamina:
	case EOryxStat::Armor:
	case EOryxStat::MovementSpeed:
	case EOryxStat::HealthRegen:
	case EOryxStat::ManaRegen:
	case EOryxStat::StaminaRegen:
	case EOryxStat::Luck:
		return false;

	default:
		return false;
	}
}

FText UOryxStatFormatLibrary::GetStatDisplayName(EOryxStat Stat)
{
	switch (Stat)
	{
	case EOryxStat::MaxHealth:         return NSLOCTEXT("Oryx", "Stat_MaxHealth", "Max Health");
	case EOryxStat::Damage:            return NSLOCTEXT("Oryx", "Stat_Damage", "Damage");
	case EOryxStat::AttackSpeed:       return NSLOCTEXT("Oryx", "Stat_AttackSpeed", "Attack Speed");
	case EOryxStat::CritChance:        return NSLOCTEXT("Oryx", "Stat_CritChance", "Critical Chance");
	case EOryxStat::CritDamage:        return NSLOCTEXT("Oryx", "Stat_CritDamage", "Critical Damage");
	case EOryxStat::PhysicalPower:     return NSLOCTEXT("Oryx", "Stat_PhysicalPower", "Physical Power");
	case EOryxStat::FireDamage:        return NSLOCTEXT("Oryx", "Stat_FireDamage", "Fire Damage");
	case EOryxStat::ColdDamage:        return NSLOCTEXT("Oryx", "Stat_ColdDamage", "Cold Damage");
	case EOryxStat::MagicDamage:       return NSLOCTEXT("Oryx", "Stat_MagicDamage", "Magic Damage");
	case EOryxStat::CurseDamage:       return NSLOCTEXT("Oryx", "Stat_CurseDamage", "Curse Damage");
	case EOryxStat::PoisonDamage:      return NSLOCTEXT("Oryx", "Stat_PoisonDamage", "Poison Damage");
	case EOryxStat::MovementSpeed:     return NSLOCTEXT("Oryx", "Stat_MovementSpeed", "Movement Speed");
	case EOryxStat::Luck:              return NSLOCTEXT("Oryx", "Stat_Luck", "Luck");
	case EOryxStat::CooldownReduction: return NSLOCTEXT("Oryx", "Stat_CDR", "Cooldown Reduction");
	case EOryxStat::Armor:             return NSLOCTEXT("Oryx", "Stat_Armor", "Armor");
	case EOryxStat::CastSpeed:         return NSLOCTEXT("Oryx", "Stat_CastSpeed", "Cast Speed");
	case EOryxStat::MaxMana:           return NSLOCTEXT("Oryx", "Stat_MaxMana", "Max Mana");
	case EOryxStat::MaxStamina:        return NSLOCTEXT("Oryx", "Stat_MaxStamina", "Max Stamina");
	case EOryxStat::HealthRegen:       return NSLOCTEXT("Oryx", "Stat_HealthRegen", "Health Regen");
	case EOryxStat::ManaRegen:         return NSLOCTEXT("Oryx", "Stat_ManaRegen", "Mana Regen");
	case EOryxStat::StaminaRegen:      return NSLOCTEXT("Oryx", "Stat_StaminaRegen", "Stamina Regen");
	case EOryxStat::DamageReduction:   return NSLOCTEXT("Oryx", "Stat_DamageReduction", "Damage Reduction");
	case EOryxStat::GoldGain:          return NSLOCTEXT("Oryx", "Stat_GoldGain", "Gold Gain");
	default:                           return NSLOCTEXT("Oryx", "Stat_Unknown", "Unknown");
	}
}

FText UOryxStatFormatLibrary::FormatStatValue(EOryxStat Stat, float Value)
{
	if (IsPercentageStat(Stat))
	{
		const int32 Pct = FMath::RoundToInt(Value * 100.f);
		return FText::FromString(FString::Printf(TEXT("%d%%"), Pct));
	}

	// Flat — show integer unless there's a meaningful fractional part (regens can be e.g. 1.5/s).
	if (FMath::IsNearlyEqual(Value, FMath::RoundToFloat(Value)))
	{
		return FText::FromString(FString::Printf(TEXT("%d"), FMath::RoundToInt(Value)));
	}
	return FText::FromString(FString::Printf(TEXT("%.1f"), Value));
}

FText UOryxStatFormatLibrary::FormatModifier(const FOryxStatModifier& Modifier)
{
	const FText StatName = GetStatDisplayName(Modifier.Stat);
	const TCHAR* Sign = (Modifier.Value >= 0.f) ? TEXT("+") : TEXT("");

	FString ValueStr;
	if (IsPercentageStat(Modifier.Stat))
	{
		const int32 Pct = FMath::RoundToInt(Modifier.Value * 100.f);
		ValueStr = FString::Printf(TEXT("%s%d%%"), Sign, Pct);
	}
	else
	{
		if (FMath::IsNearlyEqual(Modifier.Value, FMath::RoundToFloat(Modifier.Value)))
		{
			ValueStr = FString::Printf(TEXT("%s%d"), Sign, FMath::RoundToInt(Modifier.Value));
		}
		else
		{
			ValueStr = FString::Printf(TEXT("%s%.1f"), Sign, Modifier.Value);
		}
	}

	// Multiplicative modifiers stack differently — flag them so the player understands the math.
	const FString Suffix = (Modifier.Op == EOryxModOp::Multiplicative) ? TEXT(" (more)") : TEXT("");

	return FText::FromString(FString::Printf(TEXT("%s %s%s"), *ValueStr, *StatName.ToString(), *Suffix));
}

FText UOryxStatFormatLibrary::FormatRankPreview(EOryxStat Stat, float PerRankValue, int32 CurrentRank, int32 MaxRanks)
{
	const FText StatName = GetStatDisplayName(Stat);
	const bool bPct = IsPercentageStat(Stat);

	auto FormatAmount = [bPct](float Total) -> FString
	{
		if (bPct)
		{
			return FString::Printf(TEXT("+%d%%"), FMath::RoundToInt(Total * 100.f));
		}
		if (FMath::IsNearlyEqual(Total, FMath::RoundToFloat(Total)))
		{
			return FString::Printf(TEXT("+%d"), FMath::RoundToInt(Total));
		}
		return FString::Printf(TEXT("+%.1f"), Total);
	};

	const float CurrentTotal = PerRankValue * CurrentRank;

	// Maxed — no next step.
	if (CurrentRank >= MaxRanks)
	{
		return FText::FromString(FString::Printf(TEXT("%s %s (Max)"),
			*FormatAmount(CurrentTotal), *StatName.ToString()));
	}

	const float NextTotal = PerRankValue * (CurrentRank + 1);

	// Rank 0 — show only the next step ("+10% Attack Speed").
	if (CurrentRank <= 0)
	{
		return FText::FromString(FString::Printf(TEXT("%s %s"),
			*FormatAmount(NextTotal), *StatName.ToString()));
	}

	// Mid-progression — show current → next.
	return FText::FromString(FString::Printf(TEXT("%s → %s %s"),
		*FormatAmount(CurrentTotal), *FormatAmount(NextTotal), *StatName.ToString()));
}
