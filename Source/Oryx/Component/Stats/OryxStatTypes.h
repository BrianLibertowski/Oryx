#pragma once

#include "CoreMinimal.h"
#include "OryxStatTypes.generated.h"

/** All stats tracked by UOryxStatsComponent. Adding a stat = one entry here. */
UENUM(BlueprintType)
enum class EOryxStat : uint8
{
	MaxHealth          UMETA(DisplayName = "Max Health"),
	Damage             UMETA(DisplayName = "Damage"),
	AttackSpeed        UMETA(DisplayName = "Attack Speed"),
	CritChance         UMETA(DisplayName = "Critical Chance"),
	CritDamage         UMETA(DisplayName = "Critical Damage"),
	PhysicalPower      UMETA(DisplayName = "Physical Power"),
	FireDamage         UMETA(DisplayName = "Fire Damage"),
	ColdDamage         UMETA(DisplayName = "Cold Damage"),
	MagicDamage        UMETA(DisplayName = "Magic Damage"),
	CurseDamage        UMETA(DisplayName = "Curse Damage"),
	PoisonDamage       UMETA(DisplayName = "Poison Damage"),
	MovementSpeed      UMETA(DisplayName = "Movement Speed"),
	Luck               UMETA(DisplayName = "Luck"),
	CooldownReduction  UMETA(DisplayName = "Cooldown Reduction"),
	Armor              UMETA(DisplayName = "Armor"),
	CastSpeed          UMETA(DisplayName = "Cast Speed"),
	MaxMana            UMETA(DisplayName = "Max Mana"),
	MaxStamina         UMETA(DisplayName = "Max Stamina"),
	HealthRegen        UMETA(DisplayName = "Health Regen (HP/sec)"),
	ManaRegen          UMETA(DisplayName = "Mana Regen (MP/sec)"),
	StaminaRegen       UMETA(DisplayName = "Stamina Regen (SP/sec)"),
	DamageReduction    UMETA(DisplayName = "Damage Reduction (0-1)"),
	GoldGain           UMETA(DisplayName = "Gold Gain (0-1 multiplier)")
};

/** Modifier operation type. Final value = (Base + Sum(Additive)) * (1 + Sum(Multiplicative)). */
UENUM(BlueprintType)
enum class EOryxModOp : uint8
{
	Additive       UMETA(DisplayName = "Additive (+X)"),
	Multiplicative UMETA(DisplayName = "Multiplicative (+X% as 0.X)")
};

/** A single stat modifier. Items, buffs, level-ups push these into UOryxStatsComponent. */
USTRUCT(BlueprintType)
struct FOryxStatModifier
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EOryxStat Stat = EOryxStat::MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EOryxModOp Op = EOryxModOp::Additive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value = 0.f;

	/** Tag identifying who applied this modifier (item GUID, buff GUID). Used for targeted removal. */
	UPROPERTY(BlueprintReadWrite)
	FGuid SourceId;
};
