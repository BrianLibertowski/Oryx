#pragma once

#include "CoreMinimal.h"
#include "OryxStatusEffectTypes.generated.h"

/** All status effects supported by UOryxStatusEffectsComponent. */
UENUM(BlueprintType)
enum class EOryxStatusEffectType : uint8
{
	Burn   UMETA(DisplayName = "Burn"),    // DoT, Fire damage
	Poison UMETA(DisplayName = "Poison"),  // DoT, Poison damage
	Chill  UMETA(DisplayName = "Chill")    // MovementSpeed slow
};

/**
 *  Input payload for applying a status effect. Built by abilities/AI/items
 *  and passed to UOryxStatusEffectsComponent::ApplyEffect.
 *
 *  SourceId + Type form the uniqueness key. Re-applying with the same key
 *  refreshes Duration; different sources of the same Type stack.
 */
USTRUCT(BlueprintType)
struct FOryxStatusEffectSpec
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oryx|Status")
	EOryxStatusEffectType Type = EOryxStatusEffectType::Burn;

	/** Total seconds before expiry. Refreshed if same SourceId+Type re-applied. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oryx|Status")
	float Duration = 4.f;

	/** Seconds between ticks (DoT only). Ignored for stat-mod effects. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oryx|Status")
	float TickInterval = 0.5f;

	/** DoT: damage per tick. Stat-mod (Chill): magnitude as 0–1 slow fraction (e.g. 0.4 = 40% slow). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oryx|Status")
	float Magnitude = 5.f;

	/** Pawn that applied the effect. DoT damage scales with this actor's stats via the damage formula. */
	UPROPERTY(BlueprintReadWrite, Category = "Oryx|Status")
	TWeakObjectPtr<AActor> Instigator;

	/** Identifies the application source (ability instance, item GUID). Used for the (SourceId, Type) key. */
	UPROPERTY(BlueprintReadWrite, Category = "Oryx|Status")
	FGuid SourceId;
};

/** Internal runtime record of an active effect. Not exposed to BP — use the component API. */
struct FOryxActiveStatusEffect
{
	FOryxStatusEffectSpec Spec;
	float TimeRemaining = 0.f;
	float TimeSinceTick = 0.f;

	/** For stat-mod effects (Chill): GUID of the modifier we pushed into the target's StatsComponent. */
	FGuid AppliedModifierId;
};
