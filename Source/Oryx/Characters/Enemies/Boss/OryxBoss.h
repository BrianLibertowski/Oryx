#pragma once

#include "CoreMinimal.h"
#include "Characters/Enemies/Base/OryxEnemy.h"
#include "OryxBoss.generated.h"

class AOryxEnemy;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBossHealthThreshold, int32, ThresholdIndex, float, HealthFraction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossTelegraphAttack, float, WindupSeconds);

/**
 *  Base class for boss enemies (Mega Grizzlebark, future stage bosses).
 *  Extends OryxEnemy with:
 *    - HP-threshold delegate (fires once per crossed threshold, e.g. every 30% per plan D8)
 *      → BP listens and spawns followers / phase shifts.
 *    - Telegraphed attack hook (BlueprintImplementableEvent) for the slow heavy windup VFX.
 *    - Optional automatic follower spawn at each threshold (FollowerSpawnClass + count).
 *
 *  Behavior tree integration:
 *    - BT branches on bIsTelegraphingAttack to gate movement during windup.
 *    - BT calls TelegraphAttack(WindupSeconds) → broadcasts OnTelegraphAttack →
 *      BP plays VFX/anim → calls FinishTelegraphedAttack to fire the actual hit.
 */
UCLASS()
class ORYX_API AOryxBoss : public AOryxEnemy
{
	GENERATED_BODY()

public:
	AOryxBoss();

	/** Fires once when health drops below each entry in HealthThresholds (low → high index order). */
	UPROPERTY(BlueprintAssignable, Category = "Oryx|Boss")
	FOnBossHealthThreshold OnHealthThreshold;

	/** Fired when BT/AI initiates a telegraphed heavy attack. BP listens to spawn VFX/anim. */
	UPROPERTY(BlueprintAssignable, Category = "Oryx|Boss")
	FOnBossTelegraphAttack OnTelegraphAttack;

	UFUNCTION(BlueprintPure, Category = "Oryx|Boss")
	bool IsTelegraphingAttack() const { return bIsTelegraphingAttack; }

	/**
	 *  Start a telegraphed attack windup. Sets bIsTelegraphingAttack=true, broadcasts OnTelegraphAttack,
	 *  schedules FinishTelegraphedAttack after WindupSeconds. BP wires VFX/anim/sound to OnTelegraphAttack.
	 *  After windup, BP/BT can use bIsTelegraphingAttack=false as the cue to apply the real damage swing.
	 */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Boss")
	void TelegraphAttack(float WindupSeconds);

protected:
	virtual void BeginPlay() override;

	/**
	 *  Health fractions (0-1) that fire OnHealthThreshold once when crossed (descending).
	 *  Default: every 30% per plan D8. Index 0 = first crossed = 0.7.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Boss")
	TArray<float> HealthThresholds = { 0.7f, 0.4f, 0.1f };

	/** Optional: spawn followers when a threshold fires. Null = no auto-spawn. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Boss|Followers")
	TSubclassOf<AOryxEnemy> FollowerSpawnClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Boss|Followers", meta = (ClampMin = "0"))
	int32 FollowersPerThreshold = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oryx|Boss|Followers")
	float FollowerSpawnRadius = 800.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|Boss|State")
	bool bIsTelegraphingAttack = false;

	/** Tracks which thresholds have already fired this lifecycle (parallel index with HealthThresholds). */
	UPROPERTY()
	TArray<bool> ThresholdsFired;

	/** Bound to HealthComponent::OnDamaged in BeginPlay. */
	UFUNCTION()
	void HandleBossDamaged(float NewHealth, float DamageAmount);

	UFUNCTION()
	void FinishTelegraphedAttack();

private:
	void SpawnFollowers();
	FTimerHandle TelegraphTimer;
};
