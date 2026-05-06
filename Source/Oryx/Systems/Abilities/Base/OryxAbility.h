#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OryxAbility.generated.h"

class UOryxStatsComponent;

UCLASS(Abstract, Blueprintable)
class ORYX_API UOryxAbility : public UObject
{
	GENERATED_BODY()

public:
	UOryxAbility();

	/** Base cooldown in seconds between activations. Scaled by stats at runtime — see GetEffectiveCooldown. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Ability")
	float Cooldown = 1.f;

	/** Treat this ability as a basic attack (melee swing, wand bolt, bow shot). Basic attacks scale with AttackSpeed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Ability")
	bool bIsBasicAttack = false;

	// Called by UOryxAbilityComponent after the cooldown gate passes
	UFUNCTION(BlueprintNativeEvent, Category = "Oryx|Ability")
	void Activate(AActor* Owner);
	virtual void Activate_Implementation(AActor* Owner);

	/** Legacy gate — checks against the raw Cooldown. Component prefers GetEffectiveCooldown for live stat scaling. */
	UFUNCTION(BlueprintPure, Category = "Oryx|Ability")
	bool IsOnCooldown(float CurrentTime) const;

	/** Returns Cooldown scaled by stats: basic attacks divide by AttackSpeed × (1 + CDR); others by (1 + CDR). */
	UFUNCTION(BlueprintPure, Category = "Oryx|Ability")
	float GetEffectiveCooldown(const UOryxStatsComponent* Stats) const;

	UFUNCTION(BlueprintPure, Category = "Oryx|Ability")
	float GetLastActivatedTime() const { return LastActivatedTime; }

	// Stamps activation time — called by the component post-Activate
	void MarkActivated(float CurrentTime) { LastActivatedTime = CurrentTime; }

	// Needed so Blueprint world-context nodes resolve correctly on this UObject
	virtual UWorld* GetWorld() const override;

private:
	UPROPERTY(Transient)
	float LastActivatedTime = -1000.f;
};
