#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OryxStatTypes.h"
#include "OryxStatsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatsChanged);

/**
 *  Tracks a character's stats. Per-class base values configured in BP Class Defaults
 *  via the BaseStats map; runtime modifiers (items, buffs) are added/removed through
 *  Add/RemoveModifier. Fold formula: Final = (Base + SumAdditive) * (1 + SumMultiplicative).
 *
 *  Stats are run-scope: this component lives on the Character, NOT the PlayerState.
 *  PlayerState carries persistent currency/unlocks; stats reset on death.
 */
UCLASS(ClassGroup=(Oryx), meta=(BlueprintSpawnableComponent))
class ORYX_API UOryxStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOryxStatsComponent();

	/** Returns the final folded value for the given stat. */
	UFUNCTION(BlueprintPure, Category = "Oryx|Stats")
	float GetStat(EOryxStat Stat) const;

	/** Returns just the class-defined base value (no modifiers). */
	UFUNCTION(BlueprintPure, Category = "Oryx|Stats")
	float GetBaseStat(EOryxStat Stat) const;

	/** Adds a modifier. If Modifier.SourceId is invalid, a fresh GUID is assigned. Returns the GUID used. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Stats")
	FGuid AddModifier(FOryxStatModifier Modifier);

	/** Removes every modifier whose SourceId matches. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Stats")
	void RemoveModifiersBySource(FGuid SourceId);

	/** Wipes all modifiers (e.g. on respawn / new run). */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Stats")
	void ClearAllModifiers();

	UPROPERTY(BlueprintAssignable, Category = "Oryx|Stats")
	FOnStatsChanged OnStatsChanged;

protected:
	/** Class-defined base values. Configure in BP Class Defaults — entries missing here default to 0. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Stats")
	TMap<EOryxStat, float> BaseStats;

	/** Runtime modifier list. Pushed by items/buffs, popped on removal. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|Stats")
	TArray<FOryxStatModifier> Modifiers;
};
