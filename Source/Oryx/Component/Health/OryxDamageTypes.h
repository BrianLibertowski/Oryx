#pragma once

#include "CoreMinimal.h"
#include "OryxDamageTypes.generated.h"

/** Damage school. Maps 1:1 to the corresponding EOryxStat::*Damage stat used for scaling. */
UENUM(BlueprintType)
enum class EOryxDamageType : uint8
{
	Physical UMETA(DisplayName = "Physical"),
	Fire     UMETA(DisplayName = "Fire"),
	Cold     UMETA(DisplayName = "Cold"),
	Magic    UMETA(DisplayName = "Magic"),
	Curse    UMETA(DisplayName = "Curse"),
	Poison   UMETA(DisplayName = "Poison")
};

/**
 *  Payload for a single damage application. Built by the attacker (ability, AI task,
 *  status effect tick) and handed to UOryxHealthComponent::ApplyDamageEvent, which
 *  resolves the final number against target stats.
 *
 *  BaseAmount is pre-formula. Crit / armor / damage-type scaling is applied inside
 *  HealthComponent so every damage path lands on the same math.
 */
USTRUCT(BlueprintType)
struct FOryxDamageEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oryx|Damage")
	float BaseAmount = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oryx|Damage")
	EOryxDamageType DamageType = EOryxDamageType::Physical;

	/** Pawn / actor that caused the damage. May be null for environmental sources. */
	UPROPERTY(BlueprintReadWrite, Category = "Oryx|Damage")
	TWeakObjectPtr<AActor> Instigator;

	/** Specific actor that delivered the damage (projectile, hitbox). May equal Instigator for melee. */
	UPROPERTY(BlueprintReadWrite, Category = "Oryx|Damage")
	TWeakObjectPtr<AActor> Causer;

	/** Identifies the source for de-dupe / on-hit tracking (ability instance, status effect). Optional. */
	UPROPERTY(BlueprintReadWrite, Category = "Oryx|Damage")
	FGuid SourceId;
};
