#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "OryxRunSave.generated.h"

/**
 *  Mid-run save (D16). Temporary, single slot. Deleted on run end (death or extract per D19).
 *
 *  Holds:
 *    - Current stage + map
 *    - Player world transform
 *    - Run-scope resources (gold, current HP/MP/Stamina)
 *    - Owned items (by ItemId — items resolved against asset registry on load)
 *    - Active card buff modifier GUIDs (for clean revoke on death/extract)
 *    - Cleared encounter ids (so resume doesn't re-fight cleared camps per D16)
 *    - Wave Mode progress
 *    - Version int for migration
 *
 *  Save triggers (D16):
 *    - Manual save (player presses Save in pause menu)
 *    - Auto-save every 5min
 *    - On significant world events (boss defeat, stage transition)
 *
 *  Resume behavior (D16):
 *    - Current encounter resets (player respawns at last safe point or stage entry)
 *    - Cleared encounters stay cleared (ClearedEncounterIds preserved)
 *    - Active card buffs reapplied via SourceIds from ActiveCardSourceIds
 */
UCLASS()
class ORYX_API UOryxRunSave : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 Version = 1;

	/** Map this run takes place in. e.g. "WildPlains_Final". */
	UPROPERTY()
	FName CurrentMapName;

	/** Stage drives item cost (D6) + chest cost (D10). 1 / 2 / 3. */
	UPROPERTY()
	int32 CurrentStage = 1;

	UPROPERTY()
	FVector PlayerLocation = FVector::ZeroVector;

	UPROPERTY()
	FRotator PlayerRotation = FRotator::ZeroRotator;

	UPROPERTY()
	int32 Gold = 0;

	UPROPERTY()
	float CurrentHealth = 100.f;

	UPROPERTY()
	float CurrentMana = 100.f;

	UPROPERTY()
	float CurrentStamina = 100.f;

	/** Inventory by ItemId — items resolved from the asset registry on load. */
	UPROPERTY()
	TArray<FName> InventoryItemIds;

	/**
	 *  Active run-buff card modifier source IDs. These are FOryxStatModifier.SourceId values
	 *  that need to be re-applied on load (resolves the card by id from UOryxCardLibrary,
	 *  then pushes to StatsComponent with the saved GUID for clean cleanup on extract/death).
	 */
	UPROPERTY()
	TArray<FName> ActiveCardIds;

	/** Encounters cleared this run (totem/camp/outpost/elite ids). Per D16, cleared stay cleared on resume. */
	UPROPERTY()
	TArray<FName> ClearedEncounterIds;

	/** Wave Mode wave index. 0 = not in wave mode. */
	UPROPERTY()
	int32 CurrentWaveNumber = 0;

	/** Run start timestamp (seconds since epoch) — for tally screen run duration. */
	UPROPERTY()
	int64 RunStartUnixTime = 0;

	/** Slot name convention. */
	static const FString GetSlotName() { return TEXT("RunSave"); }
	static const int32 GetUserIndex() { return 0; }
};
