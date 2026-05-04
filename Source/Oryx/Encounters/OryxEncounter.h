#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OryxEncounter.generated.h"

class AOryxCharacter;
class AOryxEnemy;
class AOryxRewardPickup;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEncounterComplete, AOryxEncounter*, Encounter);

/**
 *  Abstract base for all encounter types (camps, trial totems, elite arenas).
 *  Owns: enemy spawn pool, spawn radius, reward config, lifecycle state.
 *  Subclasses supply the trigger (overlap, interact, etc.) and call StartEncounter().
 */
UCLASS(Abstract)
class ORYX_API AOryxEncounter : public AActor
{
	GENERATED_BODY()

public:
	AOryxEncounter();

	UPROPERTY(BlueprintAssignable, Category = "Oryx|Encounter")
	FOnEncounterComplete OnEncounterComplete;

	UFUNCTION(BlueprintPure, Category = "Oryx|Encounter")
	bool IsCleared() const { return bCleared; }

	UFUNCTION(BlueprintPure, Category = "Oryx|Encounter")
	bool IsActive() const { return bActive; }

protected:
	/** Possible enemy classes; one is randomly chosen per spawn slot */
	UPROPERTY(EditAnywhere, Category = "Oryx|Encounter|Spawning")
	TArray<TSubclassOf<AOryxEnemy>> EnemyPool;

	/** Total enemies to spawn for this encounter */
	UPROPERTY(EditAnywhere, Category = "Oryx|Encounter|Spawning", meta = (ClampMin = "1"))
	int32 EnemyCount = 8;

	/** Random spawn radius (cm) around this actor's location. Ignored if SpawnPoints is non-empty. */
	UPROPERTY(EditAnywhere, Category = "Oryx|Encounter|Spawning")
	float SpawnRadius = 1500.f;

	/** Optional explicit spawn points (child scene components). If set, overrides SpawnRadius. */
	UPROPERTY(EditAnywhere, Category = "Oryx|Encounter|Spawning")
	TArray<FVector> SpawnOffsets;

	// --- Rewards ---

	/** Gold awarded to the triggering player on completion. Zero = no gold. */
	UPROPERTY(EditAnywhere, Category = "Oryx|Encounter|Rewards", meta = (ClampMin = "0"))
	int32 GoldReward = 0;

	/** Reward pickup spawned at completion (interactable — player walks up + presses Interact).
	 *  Null = no pickup (e.g. totems give gold only). */
	UPROPERTY(EditAnywhere, Category = "Oryx|Encounter|Rewards")
	TSubclassOf<AOryxRewardPickup> RewardPickupClass;

	/** Local offset from this actor's location where the reward pickup spawns */
	UPROPERTY(EditAnywhere, Category = "Oryx|Encounter|Rewards")
	FVector RewardPickupOffset = FVector(0.f, 0.f, 100.f);

	// --- State ---

	UPROPERTY(VisibleAnywhere, Category = "Oryx|Encounter|State")
	bool bCleared = false;

	UPROPERTY(VisibleAnywhere, Category = "Oryx|Encounter|State")
	bool bActive = false;

	/** The player who triggered this encounter — receives rewards on complete */
	UPROPERTY(VisibleAnywhere, Category = "Oryx|Encounter|State")
	AOryxCharacter* TriggeringPlayer = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Oryx|Encounter|State")
	int32 RemainingEnemies = 0;

	/** All enemies spawned by this encounter (alive or dead). Used for shared-aggro broadcasts. */
	UPROPERTY(VisibleAnywhere, Category = "Oryx|Encounter|State")
	TArray<AOryxEnemy*> SpawnedEnemies;

	/** Subclasses call this to begin the encounter. Idempotent — does nothing if already active or cleared. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Encounter")
	void StartEncounter(AOryxCharacter* InstigatingPlayer);

	/** Force-aggros every alive enemy spawned by this encounter onto the given target. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Encounter")
	void BroadcastAlert(AActor* Target);

	/** Bound to each spawned enemy's OnDeath */
	UFUNCTION()
	void HandleEnemyDeath(AActor* DeadActor);

private:
	void CompleteEncounter();
	FVector GetNextSpawnLocation(int32 SlotIndex) const;
	TSubclassOf<AOryxEnemy> PickEnemyClass() const;
};
