#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "OryxRunManager.generated.h"

/**
 *  Run lifecycle states. The state machine is linear except WaveMode (post-boss optional).
 *
 *  InRun           — default; player exploring the biome, killing things, buying items.
 *  BossDefeated    — boss arena cleared; UI offers Extract / Wave Mode.
 *  WaveMode        — player chose endless waves after boss (D20).
 *  RunComplete     — player chose to extract OR finished wave mode peacefully.
 *  Dead            — player died. Run ends. No revive shrine in solo (D19).
 */
UENUM(BlueprintType)
enum class EOryxRunState : uint8
{
	InRun         UMETA(DisplayName = "In Run"),
	BossDefeated  UMETA(DisplayName = "Boss Defeated"),
	WaveMode      UMETA(DisplayName = "Wave Mode"),
	RunComplete   UMETA(DisplayName = "Run Complete"),
	Dead          UMETA(DisplayName = "Dead")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRunStateChanged, EOryxRunState, NewState, EOryxRunState, OldState);

/**
 *  Owns run-scope state: current stage, run lifecycle phase, run start time.
 *  Lives as the level's GameState — one per level, replicates naturally when co-op is wired.
 *
 *  Drives:
 *    - Vendor / chest pricing (CurrentStage feeds UOryxItemEconomy)
 *    - WaveMode toggle (post-boss optional grind for XP only)
 *    - End-of-run tally screen (RunComplete | Dead → final summary widget)
 *
 *  Set as GameStateClass in your GameMode constructor (AOryxGameMode).
 */
UCLASS()
class ORYX_API AOryxRunManager : public AGameStateBase
{
	GENERATED_BODY()

public:
	AOryxRunManager();

	UPROPERTY(BlueprintAssignable, Category = "Oryx|Run")
	FOnRunStateChanged OnRunStateChanged;

	UFUNCTION(BlueprintPure, Category = "Oryx|Run")
	EOryxRunState GetRunState() const { return RunState; }

	UFUNCTION(BlueprintPure, Category = "Oryx|Run")
	int32 GetCurrentStage() const { return CurrentStage; }

	UFUNCTION(BlueprintPure, Category = "Oryx|Run")
	float GetRunDurationSeconds() const;

	/** Resets state + counters, sets stage, broadcasts InRun. Call at level start / new run. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Run")
	void StartRun(int32 Stage);

	/** Called by AOryxBoss death handler. Transitions InRun → BossDefeated. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Run")
	void HandleBossDefeated();

	/** Called when player picks WaveMode on the stage-cleared screen. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Run")
	void StartWaveMode();

	/** Called when player picks Extract. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Run")
	void HandleExtract();

	/** Called by player character death handler. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Run")
	void HandlePlayerDeath();

	/** Convenience helper to update PlayerState.CurrentStage on all players. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Run")
	void PropagateStageToPlayers();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|Run")
	EOryxRunState RunState = EOryxRunState::InRun;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|Run")
	int32 CurrentStage = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Oryx|Run")
	float RunStartTime = 0.f;

	/** Internal transition helper — sets state + broadcasts. No-op if same state. */
	void SetRunState(EOryxRunState NewState);

	/**
	 *  Persists the (first) player's profile to disk and deletes the mid-run save slot.
	 *  Called from HandlePlayerDeath / HandleExtract — anything that ends the run.
	 *  Single-player demo: takes the first AOryxPlayerState. Co-op pass: iterate all + save each.
	 */
	void PersistProfileAndCleanupRun();
};
