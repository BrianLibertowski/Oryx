#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Characters/OryxClassTypes.h"
#include "OryxGameInstance.generated.h"

class UOryxProfileSave;
class AOryxPlayerState;
class UOryxSkillTree;

/**
 *  Game-instance owner of the profile save slot (UOryxProfileSave). Lifetime = process.
 *
 *  Save/load policy (D16):
 *    - LoadProfile runs in Init(); caches a UOryxProfileSave instance for the rest of the session.
 *      If no slot exists, a fresh empty profile is created so SaveProfile is always non-null.
 *    - SaveProfile writes the cached profile to disk. Called automatically from:
 *        * AOryxPlayerState::HandleLevelUp (after every level-up)
 *        * AOryxRunManager on RunComplete / Dead (run end)
 *      Manual saves from UI (pause-menu Save button) call SaveProfile via the BlueprintCallable.
 *    - ApplyProfileToPlayerState pushes loaded fields into LevelComponent + SkillTreeComponent.
 *      Called from AOryxPlayerState::BeginPlay so progression hydrates before the player moves.
 *    - DeleteRunSave clears the mid-run slot (cleanup at run end per D19).
 *
 *  Skill tree allocation rehydration TODO:
 *    SetAllocations is direct-set — it does NOT push the stored ranks back onto the live StatsComponent
 *    as modifiers. Players' XP/Level persists immediately; their invested skill points appear in the data
 *    but their associated stat bonuses won't be live until a Tree registry (TArray<UOryxSkillTree*>) is
 *    wired so ApplyProfileToPlayerState can walk allocated nodes and push their modifiers. Deferred.
 */
UCLASS()
class ORYX_API UOryxGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	UPROPERTY(BlueprintReadWrite, Category = "Oryx|Class Select")
	TSubclassOf<APawn> SelectedPawnClass;

	/**
	 *  ALL skill tree DataAssets in the game, configured on BP_OryxGameInstance Class Defaults.
	 *  Used by the save-load path to rehydrate stat modifiers from saved allocations:
	 *  AOryxCharacter::PossessedBy → SkillTreeComponent::ReapplyAllocations(ActiveClass, this array).
	 *  EVERY new UOryxSkillTree asset MUST be added here or its saved ranks restore as data-only
	 *  (points spent visible, stat bonuses missing).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Oryx|SkillTree")
	TArray<TObjectPtr<UOryxSkillTree>> RegisteredSkillTrees;

	/**
	 *  The class the player just picked on WBP_CharacterSelection. Drives XP routing
	 *  (LevelComponent::ActiveClass) and skill tree screen filtering. Defaults to Warrior;
	 *  overwritten from the profile's LastPlayedClass during LoadProfile, then overwritten
	 *  again by character select before entering the run.
	 *
	 *  WBP_CharacterSelection MUST set BOTH SelectedPawnClass and SelectedClass when player picks.
	 *  Otherwise XP routes to whichever class the profile last saved.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Oryx|Class Select")
	EOryxClass SelectedClass = EOryxClass::Warrior;

	/**
	 *  Loads (or creates if missing) the profile save and caches it. Subsequent calls return the cache.
	 *  Returns the SaveGame object — null only if SaveGame creation itself fails (engine bug).
	 */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Save")
	UOryxProfileSave* LoadProfile();

	/**
	 *  Snapshots PlayerState's LevelComponent + SkillTreeComponent into the cached profile and writes to disk.
	 *  Returns true on successful disk write. Pass-through is safe to call before the cache is populated —
	 *  LoadProfile will be called internally.
	 */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Save")
	bool SaveProfile(AOryxPlayerState* FromPlayerState);

	/**
	 *  Pushes the cached profile's per-class progression + skill allocations into the PlayerState's
	 *  components. Idempotent — safe to call multiple times. Loads the profile if not yet cached.
	 */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Save")
	void ApplyProfileToPlayerState(AOryxPlayerState* PlayerState);

	/** Returns true if a mid-run save slot exists on disk. */
	UFUNCTION(BlueprintPure, Category = "Oryx|Save")
	bool HasRunSave() const;

	/** Deletes the mid-run save slot. Called at run end (death / extract) per D19. */
	UFUNCTION(BlueprintCallable, Category = "Oryx|Save")
	void DeleteRunSave();

	/** Direct accessor for the cached profile (read-only). Useful for HUD / main-menu summary widgets. */
	UFUNCTION(BlueprintPure, Category = "Oryx|Save")
	UOryxProfileSave* GetCachedProfile() const { return CachedProfile; }

protected:
	/** Cached profile save. Loaded (or created fresh) in Init(); persists for process lifetime. */
	UPROPERTY()
	UOryxProfileSave* CachedProfile = nullptr;
};
