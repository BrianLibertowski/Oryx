#pragma once

#include "CoreMinimal.h"
#include "Encounters/OryxEncounter.h"
#include "OryxEncounter_EliteArena.generated.h"

class USphereComponent;

/**
 *  Elite Arena encounter — single elite enemy spawned when the player enters a trigger volume.
 *  Reward = guaranteed item drop at higher rarity (via reward pickup) + larger gold/XP.
 *
 *  Plan: 1-2 placed per stage. EnemyPool should hold elite-grade enemy classes; the elite
 *  is just a higher-stat variant — apply EliteScaleMultiplier to the spawned enemy's
 *  MaxHealth / Damage / mesh scale for placeholder differentiation.
 */
UCLASS()
class ORYX_API AOryxEncounter_EliteArena : public AOryxEncounter
{
	GENERATED_BODY()

public:
	AOryxEncounter_EliteArena();

protected:
	virtual void BeginPlay() override;

	/** Radius of the trigger sphere that auto-starts the encounter on player overlap (cm). */
	UPROPERTY(EditAnywhere, Category = "Oryx|Encounter|Elite")
	float TriggerRadius = 1200.f;

	/** Multipliers applied to the spawned elite's MaxHealth + AttackDamage. */
	UPROPERTY(EditAnywhere, Category = "Oryx|Encounter|Elite")
	float EliteHealthMultiplier = 4.f;

	UPROPERTY(EditAnywhere, Category = "Oryx|Encounter|Elite")
	float EliteDamageMultiplier = 1.8f;

	/** Mesh scale multiplier for visual differentiation (placeholder until elite art). */
	UPROPERTY(EditAnywhere, Category = "Oryx|Encounter|Elite")
	float EliteScaleMultiplier = 1.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* TriggerVolume;

	UFUNCTION()
	void HandleTriggerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	/** Walks SpawnedEnemies after StartEncounter and applies the elite multipliers. */
	void ApplyEliteMultipliers();
};
